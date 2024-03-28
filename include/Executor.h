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

	void addTask(const std::string& name, Node* inputNode, Node* outputNode, const RunnableFunction& func) {
		std::vector<Node*> outputNodes;
		outputNodes.push_back(outputNode);
		addTask(name, inputNode, outputNodes, func);
	}

	void addTask(const std::string& name, Node* inputNode, const std::vector<Node*>& outputNodes, const RunnableFunction& func);

	void execute();

	void terminate();

	bool isDone() const {
		return m_iMaxThreads == m_threadsFinished.load();
	}

	const Task& getTask(const std::string& name) { return m_tasks.find(name)->second; }

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

	void threadExecute(int);

};
}

