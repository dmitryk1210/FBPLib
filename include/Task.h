#pragma once


#include <atomic>
#include <cassert>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "PackageBase.h"


namespace fbp {
class Node;
class Task;

typedef std::function<void(PackageBase*, Task*)> RunnableFunction;

class Task
{
public:
	enum class WorkerInstanceIterationResult {
		PROCESSED,
		SKIPPED,
		FINALIZED,
	};

	class WorkerInstance {
	public:
		bool Process() {
			WorkerInstanceIterationResult res = m_task->workerInstanceDoTaskIteration();
			return res == WorkerInstanceIterationResult::PROCESSED;
		}
	private:
		Task* m_task = nullptr;
		friend class Task;
	};

	Task(const std::string& name);
	Task(const Task& name);
	~Task();

	void Assign(const RunnableFunction& func) { m_runnable_function = func; }

	int  GetWorkerInstancesCount() const { return m_workerInstancesCount; }
	void SetInputNode(Node* node) { m_input_node = node; }
	void SetOutputNodes(const std::vector<Node*>& nodes);
	inline void SetOutputNodes(Node* node) { 
		std::vector<Node*> tmpOutputNodes = std::vector<Node*>();
		tmpOutputNodes.push_back(node);
		SetOutputNodes(std::move(tmpOutputNodes));
	}

	Node* GetOutputNode(const std::string& nodeName = "");

	void InitWorkerInstance(WorkerInstance* pWorkerInstance) {
		assert(!pWorkerInstance->m_task);
		pWorkerInstance->m_task = this;
		++m_workerInstancesCount;
	}

	void TermWorkerInstance(WorkerInstance* pWorkerInstance) {
		assert(pWorkerInstance->m_task == this);
		pWorkerInstance->m_task = nullptr;
		--m_workerInstancesCount;
	}

	int GetAvaitingPackagesCount();

	const std::string& GetName() const { return m_name; }

	uint16_t GetThreadsLimit() { return m_threadsLimit; }
	void SetThreadsLimit(int threadsLimit) { m_threadsLimit = threadsLimit; };

private:
	Node*                        m_input_node;
	std::vector<Node*>           m_output_nodes;
	std::map<std::string, Node*> m_name_to_node;
	RunnableFunction             m_runnable_function;

	uint16_t m_threadsLimit = UINT16_MAX;

	int m_workerInstancesCount;
	std::string m_name;

	std::mutex m_finishedThreadsMtx;
	int m_finishedThreads;
	PackageBase* m_pPackageEndOfStream;

	WorkerInstanceIterationResult workerInstanceDoTaskIteration();

	void Run(PackageBase* poriginal);

	friend class WorkerInstance;
};

}

