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
	: m_input_node(nullptr)
	, m_output_nodes()
	, m_is_finalized(false)
	, m_num_threads(0)
	, m_finished_threads(0)
	, m_name(name)
	, m_pPackageEndOfStream(nullptr)
{
}

Task::Task(const Task& task)
	: m_input_node(task.m_input_node)
	, m_output_nodes(task.m_output_nodes)
	, m_is_finalized(task.m_is_finalized)
	, m_num_threads(task.m_num_threads)
	, m_finished_threads(task.m_finished_threads)
	, m_name(task.m_name)
	, m_runnable_function(task.m_runnable_function)
	, m_pPackageEndOfStream(task.m_pPackageEndOfStream)
{
}

Task::~Task()
{
	//if (m_pthread) { delete m_pthread; }
}

void Task::run(PackageBase* poriginal, PackageBase** ppresult, int& target_node) 
{
	m_runnable_function(poriginal, ppresult, target_node);
}

void Task::setOutputNodes(const std::vector<Node*>& nodes)
{
	m_output_nodes = nodes;
	for (auto it = m_output_nodes.begin(); it != m_output_nodes.end(); ++it) {
		(*it)->handleTaskAttached();
	}
}

void Task::doTask()
{
	while (!m_is_finalized)
	{
		//std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (!m_input_node) continue;
		if (m_output_nodes.empty()) continue;

		PackageBase* packageIn = m_input_node->pop();
		if (!packageIn) continue;
		if (packageIn->isLast()) 
		{
			m_pPackageEndOfStream = packageIn;
			break;
		}

		//std::stringstream ss2;
		//ss2 << " thread " << m_name << " get package\n";
		//std::cout << ss2.str();

		PackageBase* packageOut = nullptr;
		int targetNode;
		run(packageIn, &packageOut, targetNode);

		if (packageOut) 
		{
			assert(!m_is_finalized);
			m_output_nodes[targetNode]->push(packageOut);
		}
	}
	m_is_finalized = true;

	std::stringstream ss2;
	ss2 << " thread " << m_name << " finalized\n";
	std::cout << ss2.str();

	{
		std::lock_guard<std::mutex> guard(m_finished_threads_mtx);
		++m_finished_threads;
		if (m_finished_threads == m_num_threads) {
			for (auto it = m_output_nodes.begin(); it != m_output_nodes.end(); ++it)
			{
				(*it)->push(m_pPackageEndOfStream);
			}
		}
	}
}
}

