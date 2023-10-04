#pragma once


#include <functional>
#include <map>
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
	Task(const std::string& name);
	~Task();

	void assign(const RunnableFunction& func) { m_runnable_function = func; }

	void setNumThreads(int nThreads) { m_num_threads = nThreads; }
	int  getNumThreads() const { return m_num_threads; }
	void setInputNode(Node* node) { m_input_node = node; }
	void setOutputNodes(const std::vector<Node*>& nodes);
	inline void setOutputNodes(Node* node) { m_output_nodes = std::vector<Node*>(); m_output_nodes.push_back(node); }

	inline bool isFinalized() const { return m_is_finalized; }

	void doTask();

private:
	Node* m_input_node;
	std::vector<Node*> m_output_nodes;
	RunnableFunction m_runnable_function;

	int m_num_threads;
	bool m_is_finalized;
	std::string m_name;

	std::thread* m_pthread;

	void run(PackageBase* poriginal, PackageBase** ppresult, int& target_node);
};

}

