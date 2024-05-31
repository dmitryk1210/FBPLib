#include "pch.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <windows.h>

#include "Executor.h"


namespace fbp {

void Executor::ThreadExecute(int threadId)
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

	while ((pTask = m_taskPool.GetNextTask(pTask, processedPackages, wasStackEmptied)) || !m_taskPool.IsAllFinished()) {
		processedPackages = 0;
		wasStackEmptied   = false;

		if (pTask != pPrevTask) {
			if (pPrevTask) pPrevTask->TermWorkerInstance(&workerInstance);
			if (pTask) pTask->InitWorkerInstance(&workerInstance);
		}
		pPrevTask = pTask;

		start = std::chrono::high_resolution_clock::now();
		end = start;
		if (pTask) {
			while (std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() < 10000 /*true*/) {
				if (!workerInstance.Process()) {
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
				chunk.taskName = pTask->GetName();
				chunk.start = start;
				chunk.stop = end;
				localDataCollector.SubmitDataChunk(std::move(chunk));
			}
		}
		else {
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(100ms);
		}
		
	}
	if (pPrevTask) pPrevTask->TermWorkerInstance(&workerInstance);
	m_threadsFinished.fetch_add(1, std::memory_order_relaxed);
}


Task& Executor::AddTask(const std::string& name, Node* inputNode, const std::vector<Node*>& outputNodes, const RunnableFunction& func)
{
	Task taskToAdd = Task(name);
	taskToAdd.SetInputNode(inputNode);
	taskToAdd.SetOutputNodes(outputNodes);
	taskToAdd.Assign(func);
	return (m_tasks.insert(std::make_pair(name, taskToAdd))).first->second;
}

void Executor::Execute(bool collectDebugData)
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
		m_threads.push_back(std::thread([this, i]() { this->ThreadExecute(i); }));
		DWORD_PTR dw = SetThreadAffinityMask(m_threads[i].native_handle(), DWORD_PTR(1) << (i % m_iMaxThreads));
		if (dw == 0) {
			DWORD dwErr = GetLastError();
			std::cerr << "SetThreadAffinityMask failed, GLE=" << dwErr << '\n';
		}
	}

}

void Executor::PrintDebugData(const char* filename) {
	std::ofstream outfile;
	outfile.open(filename, std::ios::out | std::ios::trunc);
	m_dataCollector.PrintCollectedData(outfile);
	outfile.close();
}

void Executor::Terminate()
{
	for (int i = 0; i < m_threads.size(); ++i) {
		m_threads[i].join();
	}
	m_threads.clear();
	m_threadDatas.clear();
}
}

