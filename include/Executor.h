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

	Node& AddNode(const std::string& name, bool isInitial = false);

	Task& AddTask(const std::string& name, const std::string& inputNode, const std::string& outputNode, const RunnableFunction& func) {
		std::vector<std::string> outputNodes;
		outputNodes.push_back(outputNode);
		return AddTask(name, inputNode, std::move(outputNodes), RunnableFunction(func));
	}
	Task& AddTask(const std::string& name, const std::string& inputNode, const std::string& outputNode, RunnableFunction&& func) {
		std::vector<std::string> outputNodes;
		outputNodes.push_back(outputNode);
		return AddTask(name, inputNode, std::move(outputNodes), func);
	}
	Task& AddTask(const std::string& name, const std::string& inputNode, const std::vector<std::string>& outputNodes, const RunnableFunction& func) {
		return AddTask(name, inputNode, std::vector<std::string>(outputNodes), RunnableFunction(func));
	}
	Task& AddTask(const std::string& name, const std::string& inputNode, const std::vector<std::string>& outputNodes, RunnableFunction&& func);

	void Execute();
	void Await();
	void ExecuteAndAwait();
	void PrintDebugData(const char* filename);

	void Terminate();

	bool IsDone() const {
		return m_iMaxThreads == m_threadsFinished.load();
	}

	inline Task& GetTask(const std::string& name) { 
		auto it = m_tasks.find(name);
		assert(it != m_tasks.end());
		return it->second;
	}
	inline Node& GetNode(const std::string& name) { 
		auto it = m_nodes.find(name); 
		assert(it != m_nodes.end()); 
		return it->second; 
	}


private:

	std::map<std::string, fbp::Task> m_tasks;
	std::map<std::string, fbp::Node> m_nodes;

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

