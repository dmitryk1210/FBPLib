#pragma once

#include <iostream>
#include <string>

#include "SafeQueue/SafeQueue.hpp"

#include "PackageBase.h"


namespace fbp {
class Task;

class Node
{
public:
	Node(const std::string& name);

	inline void push(PackageBase* ppackage)
	{
		if (ppackage->isLast()) {
			handleTaskDetached();
			if (this->isActive()) {
				return;
			}
		}
		m_package_queue.Produce(std::move(ppackage));
	}
	inline PackageBase* pop()
	{
		PackageBase* ppackage;
		bool isSuccess = m_package_queue.Consume(ppackage);
		return isSuccess ? ppackage : nullptr;
	}

	inline int count() { return m_package_queue.Size(); }

	inline void handleTaskAttached() { ++m_task_counter; }
	inline void handleTaskDetached() { m_task_counter = std::max(0, m_task_counter - 1); }

	inline bool isActive() const { return m_task_counter > 0; }

private:
	SafeQueue<PackageBase*> m_package_queue;
	std::string m_name;

	int m_task_counter;
};

}

