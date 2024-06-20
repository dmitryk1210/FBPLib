#include "pch.h"

#include <chrono>
#include <fstream>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

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

#ifdef FBP_ENABLE_DATA_COLLECTOR
	DataCollectorInstance& localDataCollector = m_dataCollector.CreateInstance();
#endif // FBP_ENABLE_DATA_COLLECTOR

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

#ifdef FBP_ENABLE_DATA_COLLECTOR
			DataChunk chunk;
			chunk.threadId = threadId;
			chunk.taskName = pTask->GetName();
			chunk.start = start;
			chunk.stop = end;
			localDataCollector.SubmitDataChunk(std::move(chunk));
#endif // FBP_ENABLE_DATA_COLLECTOR
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


void Executor::Execute()
{
	int numThreads = 0;
	for (auto it = m_tasks.begin(); it != m_tasks.end(); ++it) {
		m_taskPool.AddTask(it->first, &(it->second));
	}

	m_threads.clear();
	m_threadDatas.clear();
	m_threads.reserve(numThreads);
	m_threadDatas.reserve(numThreads);

#ifdef FBP_ENABLE_DATA_COLLECTOR
	m_dataCollector.SetStartTimePoint(std::chrono::high_resolution_clock::now());
#endif // FBP_ENABLE_DATA_COLLECTOR

	for (uint16_t i = 0; i < m_iMaxThreads; ++i) {
		m_threads.push_back(std::thread([this, i]() { this->ThreadExecute(i); }));
#ifdef _WIN32
		DWORD_PTR dw = SetThreadAffinityMask(m_threads[i].native_handle(), DWORD_PTR(1) << (i % m_iMaxThreads));
		if (dw == 0) {
			DWORD dwErr = GetLastError();
			std::cerr << "SetThreadAffinityMask failed, GLE=" << dwErr << '\n';
		}
#endif // _WIN32
	}
}


void Executor::Await()
{
	for (int i = 0; i < m_threads.size(); ++i) {
		if (m_threads[i].joinable()) {
			m_threads[i].join();
		}
	}
}


void Executor::ExecuteAndAwait()
{
	Execute();
	Await();
}


void Executor::PrintDebugData(const char* filename) {
#ifdef FBP_ENABLE_DATA_COLLECTOR
	std::ofstream outfile;
	outfile.open(filename, std::ios::out | std::ios::trunc);
	m_dataCollector.PrintCollectedData(outfile);
	outfile.close();
#endif // FBP_ENABLE_DATA_COLLECTOR
}


void Executor::Terminate()
{
	Await();
	m_threads.clear();
	m_threadDatas.clear();
}
}

