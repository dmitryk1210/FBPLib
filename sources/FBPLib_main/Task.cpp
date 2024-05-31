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
	, m_name_to_node         ()
	, m_workerInstancesCount (0)
	, m_finishedThreads      (0)
	, m_name                 (name)
	, m_pPackageEndOfStream  (nullptr)
{
}

Task::Task(const Task& task)
	: m_input_node           (task.m_input_node)
	, m_output_nodes         (task.m_output_nodes)
	, m_name_to_node         (task.m_name_to_node)
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

void Task::Run(PackageBase* poriginal) 
{
	m_runnable_function(poriginal, this);
}

void Task::SetOutputNodes(const std::vector<Node*>& nodes)
{
	m_output_nodes = nodes;
	m_name_to_node.clear();
	for (auto it = m_output_nodes.begin(); it != m_output_nodes.end(); ++it) {
		m_name_to_node[(*it)->GetName()] = *it;
		(*it)->HandleTaskAttached();
	}
}

Node* Task::GetOutputNode(const std::string& nodeName)
{
	if (m_output_nodes.empty()) return nullptr;
	if (nodeName == "") return m_output_nodes[0];
	auto itNode = m_name_to_node.find(nodeName);
	if (itNode == m_name_to_node.cend()) return nullptr;
	return itNode->second;
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

	Run(packageIn);

	return WorkerInstanceIterationResult::PROCESSED;
}

}

