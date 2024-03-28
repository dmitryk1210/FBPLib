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

typedef std::function<void(PackageBase*, PackageBase**, int&)> RunnableFunction;

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
		bool process() {
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

	void assign(const RunnableFunction& func) { m_runnable_function = func; }

	int  getWorkerInstancesCount() const { return m_workerInstancesCount; }
	void setInputNode(Node* node) { m_input_node = node; }
	void setOutputNodes(const std::vector<Node*>& nodes);
	inline void setOutputNodes(Node* node) { m_output_nodes = std::vector<Node*>(); m_output_nodes.push_back(node); }

	inline bool isFinalized() const { return m_is_finalized; }

	inline bool isFinalizeInProcess() const { return m_processFinalize; }

	void initWorkerInstance(WorkerInstance* pWorkerInstance) {
		if (pWorkerInstance->m_task) {
			int ighf = 0;
		}
		assert(!pWorkerInstance->m_task);
		pWorkerInstance->m_task = this;
		++m_workerInstancesCount;
	}

	void termWorkerInstance(WorkerInstance* pWorkerInstance) {
		assert(pWorkerInstance->m_task == this);
		pWorkerInstance->m_task = nullptr;
		--m_workerInstancesCount;
		//finalizeInstance();
	}

	int getAvaitingPackagesCount();

	const std::string& getName() const { return m_name; }

private:
	Node* m_input_node;
	std::vector<Node*> m_output_nodes;
	RunnableFunction m_runnable_function;

	int m_workerInstancesCount;
	bool m_processFinalize;
	bool m_is_finalized;
	std::string m_name;

	std::mutex m_finishedThreadsMtx;
	int m_finishedThreads;
	PackageBase* m_pPackageEndOfStream;

	WorkerInstanceIterationResult workerInstanceDoTaskIteration();

	void run(PackageBase* poriginal, PackageBase** ppresult, int& target_node);

	void finalizeInstance();

	friend class WorkerInstance;
};

}

