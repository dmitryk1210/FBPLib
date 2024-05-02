#include "pch.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

#include "Task.h"
#include "Node.h"


namespace fbp {
Task::Task(const std::string& name)
	: m_input_node           (nullptr)
	, m_output_nodes         ()
	, m_workerInstancesCount (0)
	, m_finishedThreads      (0)
	, m_name                 (name)
	, m_pPackageEndOfStream  (nullptr)
{
}

Task::Task(const Task& task)
	: m_input_node           (task.m_input_node)
	, m_output_nodes         (task.m_output_nodes)
	, m_workerInstancesCount (task.m_workerInstancesCount)
	, m_finishedThreads      (task.m_finishedThreads)
	, m_name                 (task.m_name)
	, m_runnable_function    (task.m_runnable_function)
	, m_pPackageEndOfStream  (task.m_pPackageEndOfStream)
{
}

Task::~Task()
{
}

void Task::Run(PackageBase* poriginal, PackageBase** ppresult, int& target_node) 
{
	m_runnable_function(poriginal, ppresult, target_node);
}

void Task::SetOutputNodes(const std::vector<Node*>& nodes)
{
	m_output_nodes = nodes;
	for (auto it = m_output_nodes.begin(); it != m_output_nodes.end(); ++it) {
		(*it)->HandleTaskAttached();
	}
}

int Task::GetAvaitingPackagesCount() { 
	return m_input_node->Count(); 
}

Task::WorkerInstanceIterationResult Task::workerInstanceDoTaskIteration()
{
	if (!m_input_node) {
		return WorkerInstanceIterationResult::SKIPPED;
	}
	if (m_output_nodes.empty()) {
		return WorkerInstanceIterationResult::SKIPPED;
	}

	PackageBase* packageIn = m_input_node->Pop();
	if (!packageIn) {
		return WorkerInstanceIterationResult::SKIPPED;
	}
	if (packageIn->IsLast()) {
		m_pPackageEndOfStream = packageIn;
		
		return WorkerInstanceIterationResult::FINALIZED;
	}

	PackageBase* packageOut = nullptr;
	int targetNode;
	Run(packageIn, &packageOut, targetNode);

	if (packageOut) {
		assert(!m_is_finalized);
		m_output_nodes[targetNode]->Push(packageOut);
	}

	return WorkerInstanceIterationResult::PROCESSED;
}

}

