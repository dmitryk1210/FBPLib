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

	Executor(bool bManualThreadDistribution = false, int iMaxThreads = -1)
		: m_bManualThreadDistribution(bManualThreadDistribution) {
		m_iMaxThreads = (iMaxThreads > 0) ? iMaxThreads : std::thread::hardware_concurrency();
	}

	void AddTask(const std::string& name, Node* inputNode, Node* outputNode, const RunnableFunction& func) {
		std::vector<Node*> outputNodes;
		outputNodes.push_back(outputNode);
		AddTask(name, inputNode, outputNodes, func);
	}

	void AddTask(const std::string& name, Node* inputNode, const std::vector<Node*>& outputNodes, const RunnableFunction& func);

	void Execute(bool collectDebugData = false);
	void PrintDebugData(const char* filename);

	void Terminate();

	bool IsDone() const {
		return m_iMaxThreads == m_threadsFinished.load();
	}

	const Task& GetTask(const std::string& name) { return m_tasks.find(name)->second; }

private:
	struct ThreadData {
		int id = -1;
		Task* pTask = nullptr;

		bool forceStopThread = false;
	};
	
	bool m_bManualThreadDistribution;
	

	std::map<std::string, fbp::Task> m_tasks;

	std::vector<std::thread> m_threads;
	size_t                   m_iMaxThreads;
	std::atomic<size_t>      m_threadsFinished{ 0 };
	std::vector<ThreadData>  m_threadDatas;

	TaskPool m_taskPool;
	DataCollector m_dataCollector;

	void ThreadExecute(int);

};
}

