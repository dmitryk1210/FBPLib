#include "pch.h"

#include "fbpGlobalDefines.h"

#include <chrono>
#include <fstream>
#include <iostream>

#include "os.h"
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

	while ((pTask = m_taskPool.GetNextTask(pTask, processedPackages, wasStackEmptied)) || !m_taskPool.IsAllFinished() || !IsPackageStreamStopped()) {
		processedPackages = 0;
		wasStackEmptied   = false;

		if (pTask != pPrevTask) {
			if (pPrevTask) pPrevTask->TermWorkerInstance(&workerInstance);
			if (pTask) pTask->InitWorkerInstance(&workerInstance);
		}
		pPrevTask = pTask;

		start = std::chrono::steady_clock::now();
		end = start;
		if (pTask) {
			using namespace std::chrono_literals;
			while (end - start < 10ms) {
				if (!workerInstance.Process()) {
					wasStackEmptied = true;
					end = std::chrono::steady_clock::now();
					break;
				}
				processedPackages++;
				end = std::chrono::steady_clock::now();
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
			std::this_thread::sleep_for(10ms);
		}
		
	}
	if (pPrevTask) pPrevTask->TermWorkerInstance(&workerInstance);
	m_threadsFinished.fetch_add(1, std::memory_order_relaxed);
}


void Executor::SetMaxThreads(int iMaxThreads) 
{
	if (m_executeInProgress) {
		assert(false && "Cannot use SetMaxThreads if program is executing");
		return;
	}
	m_iMaxThreads = iMaxThreads > 0 && iMaxThreads < std::thread::hardware_concurrency()
		? (iMaxThreads)
		: std::thread::hardware_concurrency();
}

void Executor::SetInitialNode(Node* initialNode) 
{
	if (m_initialNode) {
		m_initialNode->OnGetLast.unsubscribe(this);
	}
	m_initialNode = initialNode;
	if (m_initialNode) {
		m_initialNode->OnGetLast.subscribe(this, [this]() {
			m_packageStreamStopped.store(true, std::memory_order_relaxed);
			});
	}
}

Node& Executor::AddNode(const std::string& name, bool isInitial)
{
	Node* pNode = &(m_nodes.insert(std::make_pair(name, Node(name)))).first->second;
	if (isInitial) {
		SetInitialNode(pNode);
	}
	return *pNode;
}

Task& Executor::AddTask(const std::string& name, const std::string& inputNode, const std::vector<std::string>& outputNodes, RunnableFunction&& func)
{
	Task taskToAdd = Task(name);
	taskToAdd.SetInputNode(&GetNode(inputNode));
	std::vector<Node*> outputNodePtrs;
	for (auto it = outputNodes.cbegin(); it != outputNodes.cend(); ++it) {
		outputNodePtrs.push_back(&GetNode(*it));
	}
	taskToAdd.SetOutputNodes(std::move(outputNodePtrs));
	taskToAdd.Assign(func);
	return (m_tasks.insert(std::make_pair(name, std::move(taskToAdd)))).first->second;
}


void Executor::Execute()
{
	m_executeInProgress = true;
	int numThreads = 0;
	for (auto it = m_tasks.begin(); it != m_tasks.end(); ++it) {
		m_taskPool.AddTask(it->first, &(it->second));
	}

	m_threads.clear();
	m_threads.reserve(numThreads);

#ifdef FBP_ENABLE_DATA_COLLECTOR
	m_dataCollector.SetStartTimePoint(std::chrono::high_resolution_clock::now());
#endif // FBP_ENABLE_DATA_COLLECTOR

	for (uint16_t i = 0; i < m_iMaxThreads; ++i) {
		m_threads.push_back(std::thread([this, i]() { this->ThreadExecute(i); }));
		uint32_t err = osSetThreadAffinityMask(m_threads[i], (1u << i));
		if (err) {
			std::cerr << "osSetThreadAffinityMask failed, error=" << err << '\n';
		}

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


void Executor::PrintDebugData(const std::string& filename) {
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
	if (m_initialNode) {
		m_initialNode->OnGetLast.unsubscribe(this);
	}
}
}

