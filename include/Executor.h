#pragma once

#include <atomic>
#include <cstdint>
#include <thread>

#include "DataCollector.h"
#include "Task.h"
#include "TaskPool.h"


namespace fbp {
class Executor
{
public:

	Executor(int iMaxThreads = -1) {
		m_iMaxThreads = (iMaxThreads > 0) ? iMaxThreads : std::thread::hardware_concurrency();
	}

	Task& AddTask(const std::string& name, Node* inputNode, Node* outputNode, const RunnableFunction& func) {
		std::vector<Node*> outputNodes;
		outputNodes.push_back(outputNode);
		return AddTask(name, inputNode, outputNodes, func);
	}

	Task& AddTask(const std::string& name, Node* inputNode, const std::vector<Node*>& outputNodes, const RunnableFunction& func);

	void Execute();
	void Await();
	void ExecuteAndAwait();
	void PrintDebugData(const char* filename);

	void Terminate();

	bool IsDone() const {
		return m_iMaxThreads == m_threadsFinished.load();
	}

	Task& GetTask(const std::string& name) { return m_tasks.find(name)->second; }

private:
	struct ThreadData {
		int id = -1;
		Task* pTask = nullptr;

		bool forceStopThread = false;
	};
	

	std::map<std::string, fbp::Task> m_tasks;

	std::vector<std::thread> m_threads;
	uint16_t                 m_iMaxThreads;
	std::atomic<uint16_t>    m_threadsFinished{ 0u };
	std::vector<ThreadData>  m_threadDatas;

	TaskPool m_taskPool;
#ifdef FBP_ENABLE_DATA_COLLECTOR
	DataCollector m_dataCollector;
#endif // #ifdef FBP_ENABLE_DATA_COLLECTOR

	void ThreadExecute(int);

};
}

