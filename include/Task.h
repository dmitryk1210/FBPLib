#pragma once

#include "fbpGlobalDefines.h"

#include <atomic>
#include <cassert>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "PackageBase.h"


namespace fbp {
class Node;
class Task;

typedef std::unique_ptr<fbp::PackageBase> uptr_PackageBase;
typedef std::function<void(uptr_PackageBase, Task*)> RunnableFunction;

template <typename Derived, typename Base>
std::unique_ptr<Derived> uniquePtrCast(std::unique_ptr<Base> ptr) {
	return std::unique_ptr<Derived>(static_cast<Derived*>(ptr.release()));
}


class Task
{
private:
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

public:
	Task(const std::string& name);
	Task(const Task& name);
	~Task();

	void Assign(const RunnableFunction& func) { m_runnable_function = func; }
	void Assign(RunnableFunction&& func) { m_runnable_function = func; }

	Node* GetOutputNode(const std::string& nodeName = "");

	int GetAvaitingPackagesCountApprox();

	const std::string& GetName() const { return m_name; }

	uint16_t GetThreadsLimit() { return m_threadsLimit; }
	void SetThreadsLimit(uint16_t threadsLimit) { m_threadsLimit = threadsLimit; };

private:
	Node*                        m_input_node;
	std::vector<Node*>           m_output_nodes;
	std::map<std::string, Node*> m_name_to_node;
	RunnableFunction             m_runnable_function;

	uint16_t m_threadsLimit = UINT16_MAX;

	int m_workerInstancesCount;
	std::string m_name;

	void SetInputNode(Node* node) { m_input_node = node; }
	void SetOutputNodes(std::vector<Node*>&& nodes);
	inline void SetOutputNodes(const std::vector<Node*>& nodes) {
		SetOutputNodes(std::vector<Node*>(nodes));
	}
	void SetOutputNodes(Node* node);

	WorkerInstanceIterationResult workerInstanceDoTaskIteration();

	int  GetWorkerInstancesCount() const { return m_workerInstancesCount; }
	void InitWorkerInstance(WorkerInstance* pWorkerInstance);
	void TermWorkerInstance(WorkerInstance* pWorkerInstance);

	void Run(std::unique_ptr<PackageBase> poriginal);

	friend class Executor;
	friend class WorkerInstance;
};

}

