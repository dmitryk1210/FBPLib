#pragma once

#include <iostream>
#include <string>

#include "SafeQueue/SafeQueue.hpp"

#include "PackageBase.h"


namespace fbp {
class Task;
class TaskManager;

class Node
{
public:
	Node(const std::string& name);
	inline void push(PackageBase* ppackage) 
	{ 
		m_package_queue.Produce(std::move(ppackage)); 
	}
	inline PackageBase* pop() 
	{ 
		PackageBase* ppackage;
		bool isSuccess = m_package_queue.Consume(ppackage);
		return isSuccess ? ppackage : nullptr;
	}

	inline int count() { return m_package_queue.Size(); }

private:
	std::string m_name;
	SafeQueue<PackageBase*> m_package_queue;
	Task* m_connected_task;

	friend class TaskManager;
};

}

