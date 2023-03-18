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
	, m_pthread(nullptr)
	, m_is_finalized(false)
	, m_name(name)
{
}

Task::~Task()
{
	//if (m_pthread) { delete m_pthread; }
}

void Task::start()
{
	assert(!m_pthread && "m_pthread must be nullptr");
	m_pthread = new std::thread([this]() { this->threadFunction(); });
}

void Task::run(PackageBase* poriginal, PackageBase** ppresult, int& target_node) 
{
	m_runnable_function(poriginal, ppresult, target_node);
}

void Task::threadFunction()
{
	while (true)
	{
		//std::this_thread::sleep_for(std::chrono::milliseconds(1));

		//std::stringstream ss;
		//ss << " thread " << m_name << " is working\n";
		//std::cout << ss.str();

		if (!m_input_node) continue;
		if (m_output_nodes.empty()) continue;

		PackageBase* packageIn = m_input_node->pop();
		if (!packageIn) continue;
		if (packageIn->isLast())
		{
			for (auto it = m_output_nodes.begin(); it != m_output_nodes.end(); ++it)
			{
				(*it)->push(packageIn);
			}
			break;
		}

		//std::stringstream ss2;
		//ss2 << " thread " << m_name << " get package\n";
		//std::cout << ss2.str();


		PackageBase* packageOut = nullptr;
		int targetNode;
		run(packageIn, &packageOut, targetNode);

		m_output_nodes[targetNode]->push(packageOut);
	}

	m_is_finalized = true;
}
}

