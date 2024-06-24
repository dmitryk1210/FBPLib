#pragma once

#include "fbpGlobalDefines.h"

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <thread>

#include "DataCollector.h"
#include "Node.h"
#include "Task.h"
#include "TaskPool.h"


namespace fbp {
class Executor
{
public:

	Executor()
		: m_iMaxThreads(std::thread::hardware_concurrency()) { }

	void     SetMaxThreads(int iMaxThreads = -1);
	uint16_t GetMaxThreads() { return m_iMaxThreads; };

	void SetInitialNode(Node* initialNode);

	Task& AddTask(const std::string& name, Node* inputNode, Node* outputNode, const RunnableFunction& func) {
		std::vector<Node*> outputNodes;
		outputNodes.push_back(outputNode);
		return AddTask(name, inputNode, std::move(outputNodes), RunnableFunction(func));
	}
	Task& AddTask(const std::string& name, Node* inputNode, Node* outputNode, RunnableFunction&& func) {
		std::vector<Node*> outputNodes;
		outputNodes.push_back(outputNode);
		return AddTask(name, inputNode, std::move(outputNodes), func);
	}
	Task& AddTask(const std::string& name, Node* inputNode, const std::vector<Node*>& outputNodes, const RunnableFunction& func) {
		return AddTask(name, inputNode, std::vector<Node*>(outputNodes), RunnableFunction(func));
	}
	Task& AddTask(const std::string& name, Node* inputNode, const std::vector<Node*>& outputNodes, RunnableFunction&& func) {
		return AddTask(name, inputNode, std::vector<Node*>(outputNodes), RunnableFunction(func));
	}
	Task& AddTask(const std::string& name, Node* inputNode, std::vector<Node*>&& outputNodes, const RunnableFunction& func) {
		return AddTask(name, inputNode, outputNodes, RunnableFunction(func));
	}
	Task& AddTask(const std::string& name, Node* inputNode, std::vector<Node*>&& outputNodes, RunnableFunction&& func);

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

	std::map<std::string, fbp::Task> m_tasks;

	std::vector<std::thread> m_threads;
	uint16_t                 m_iMaxThreads;
	std::atomic<uint16_t>    m_threadsFinished{ 0u };

	Node*                    m_initialNode = nullptr;
	std::atomic<bool>        m_packageStreamStopped = false;
	bool                     m_executeInProgress    = false;

	TaskPool m_taskPool;
#ifdef FBP_ENABLE_DATA_COLLECTOR
	DataCollector m_dataCollector;
#endif // #ifdef FBP_ENABLE_DATA_COLLECTOR

	bool IsPackageStreamStopped() { return m_packageStreamStopped.load(); }
	void ThreadExecute(int);

};
}

