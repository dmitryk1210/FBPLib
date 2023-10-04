#pragma once

#include <cstdint>
#include <thread>

#include "Task.h"


namespace fbp {
class Executor
{
public:

	Executor(bool bManualThreadDistribution = false, int iMaxThreads = -1)
		: m_bManualThreadDistribution(bManualThreadDistribution)
	{
		m_iMaxThreads = (iMaxThreads > 0) ? iMaxThreads : std::thread::hardware_concurrency();
	}

	void addTask(const std::string& name, Node* inputNode, Node* outputNode, int numThreads, const RunnableFunction& func)
	{
		std::vector<Node*> outputNodes;
		outputNodes.push_back(outputNode);
		addTask(name, inputNode, outputNodes, numThreads, func);
	}

	void addTask(const std::string& name, Node* inputNode, const std::vector<Node*>& outputNodes, int numThreads, const RunnableFunction& func);

	void execute();

	void terminate();

	const Task& getTask(const std::string& name) { return m_tasks.find(name)->second; }

private:
	struct ThreadData {
		int id = -1;
		Task* pTask = nullptr;

		bool forceStopThread = false;
	};
	
	bool m_bManualThreadDistribution;
	int m_iMaxThreads;

	std::map<std::string, fbp::Task> m_tasks;

	std::vector<std::thread> m_threads;
	std::vector<ThreadData>  m_threadDatas;

	void threadExecute(ThreadData&);
};
}

