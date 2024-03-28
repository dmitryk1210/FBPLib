#include "pch.h"

#include <chrono>
#include <iostream>
#include <windows.h>

#include "Executor.h"


namespace fbp {

void Executor::threadExecute(int threadId)
{
	Task* pTask             = nullptr;
	Task* pPrevTask         = nullptr;
	bool  wasStackEmptied   = false;
	int   processedPackages = 0;
	Task::WorkerInstance workerInstance;
	std::chrono::steady_clock::time_point start_global;
	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point end;

	DataCollectorInstance& localDataCollector = m_dataCollector.CreateInstance();

	while (pTask = m_taskPool.GetNextTask(pTask, processedPackages, wasStackEmptied)) {
		processedPackages = 0;
		wasStackEmptied   = false;

		if (pTask != pPrevTask) {
			if (pPrevTask) pPrevTask->termWorkerInstance(&workerInstance);
			pTask->initWorkerInstance(&workerInstance);
		}
		pPrevTask = pTask;

		start = std::chrono::high_resolution_clock::now();
		end = start;
		while (/*std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() < 100*/ true) {
			if (!workerInstance.process()) {
				wasStackEmptied = true;
				end = std::chrono::high_resolution_clock::now();
				break;
			}
			processedPackages++;
			end = std::chrono::high_resolution_clock::now();
		}
		{
			DataChunk chunk;
			chunk.threadId = threadId;
			chunk.taskName = pTask->getName();
			chunk.start = start;
			chunk.stop = end;
			localDataCollector.SubmitDataChunk(chunk);
		}
		
	}
	if (pPrevTask) pPrevTask->termWorkerInstance(&workerInstance);
	m_threadsFinished.fetch_add(1, std::memory_order_relaxed);
}


void Executor::addTask(const std::string& name, Node* inputNode, const std::vector<Node*>& outputNodes, const RunnableFunction& func)
{
	Task taskToAdd = Task(name);
	taskToAdd.setInputNode(inputNode);
	taskToAdd.setOutputNodes(outputNodes);
	taskToAdd.assign(func);
	m_tasks.insert(std::make_pair(name, taskToAdd));
}

void Executor::execute()
{
	int numThreads = 0;
	for (auto it = m_tasks.begin(); it != m_tasks.end(); ++it) {
		m_taskPool.AddTask(it->first, &(it->second));
	}

	m_threads.clear();
	m_threadDatas.clear();
	m_threads.reserve(numThreads);
	m_threadDatas.reserve(numThreads);

	m_dataCollector.SetStartTimePoint(std::chrono::high_resolution_clock::now());

	for (int i = 0; i < m_iMaxThreads; ++i) {
		m_threads.push_back(std::thread([this, i]() { this->threadExecute(i); }));
		DWORD_PTR dw = SetThreadAffinityMask(m_threads[i].native_handle(), DWORD_PTR(1) << (i % m_iMaxThreads));
		if (dw == 0) {
			DWORD dwErr = GetLastError();
			std::cerr << "SetThreadAffinityMask failed, GLE=" << dwErr << '\n';
		}
	}

}
void Executor::terminate()
{
	for (int i = 0; i < m_threads.size(); ++i) {
		m_threads[i].join();
	}
	m_threads.clear();
	m_threadDatas.clear();
}
}

