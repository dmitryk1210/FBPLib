// Task.cpp
//

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
	, m_name                 (name)
	, m_threadsLimit         (UINT16_MAX)
{
}

Task::Task(const Task& task)
	: m_input_node           (task.m_input_node)
	, m_output_nodes         (task.m_output_nodes)
	, m_name_to_node         (task.m_name_to_node)
	, m_workerInstancesCount (task.m_workerInstancesCount)
	, m_name                 (task.m_name)
	, m_runnable_function    (task.m_runnable_function)
	, m_threadsLimit         (task.m_threadsLimit)
{
}

Task::~Task()
{
}

void Task::Run(std::unique_ptr<PackageBase> poriginal) 
{
	m_runnable_function(std::move(poriginal), this);
}

void Task::SetOutputNodes(std::vector<Node*>&& nodes)
{
	m_output_nodes = nodes;
	m_name_to_node.clear();
	for (auto it = m_output_nodes.begin(); it != m_output_nodes.end(); ++it) {
		m_name_to_node[(*it)->GetName()] = *it;
	}
}

void Task::SetOutputNodes(Node* node) {
	std::vector<Node*> tmpOutputNodes = std::vector<Node*>();
	tmpOutputNodes.push_back(node);
	SetOutputNodes(std::move(tmpOutputNodes));
}

Node* Task::GetOutputNode(const std::string& nodeName)
{
	if (m_output_nodes.empty()) return nullptr;
	if (nodeName == "") return m_output_nodes[0];
	auto itNode = m_name_to_node.find(nodeName);
	if (itNode == m_name_to_node.cend()) return nullptr;
	return itNode->second;
}

void Task::InitWorkerInstance(WorkerInstance* pWorkerInstance) {
	assert(!pWorkerInstance->m_task);
	pWorkerInstance->m_task = this;
	++m_workerInstancesCount;
}

void Task::TermWorkerInstance(WorkerInstance* pWorkerInstance) {
	assert(pWorkerInstance->m_task == this);
	pWorkerInstance->m_task = nullptr;
	--m_workerInstancesCount;
}

int Task::GetAvaitingPackagesCountApprox() { 
	return m_input_node->CountApprox(); 
}

Task::WorkerInstanceIterationResult Task::workerInstanceDoTaskIteration()
{
	if (!m_input_node) {
		return WorkerInstanceIterationResult::SKIPPED;
	}
	if (m_output_nodes.empty()) {
		return WorkerInstanceIterationResult::SKIPPED;
	}

	std::unique_ptr<PackageBase> packageIn = m_input_node->Pop();
	if (!packageIn) {
		return WorkerInstanceIterationResult::SKIPPED;
	}
	if (packageIn->IsLast()) {
		assert(false);
		return WorkerInstanceIterationResult::FINALIZED;
	}

	Run(std::move(packageIn));

	return WorkerInstanceIterationResult::PROCESSED;
}

}

