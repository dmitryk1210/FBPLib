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

	inline void Push(PackageBase* ppackage)
	{
		if (ppackage->IsLast()) {
			HandleTaskDetached();
			if (this->IsActive()) {
				return;
			}
		}
		m_package_queue.Produce(std::move(ppackage));
	}
	inline PackageBase* Pop()
	{
		PackageBase* ppackage;
		bool isSuccess = m_package_queue.Consume(ppackage);
		return isSuccess ? ppackage : nullptr;
	}

	inline int Count() { return m_package_queue.Size(); }

	inline void HandleTaskAttached() { ++m_task_counter; }
	inline void HandleTaskDetached() { m_task_counter = std::max(0, m_task_counter - 1); }

	inline bool IsActive() const { return m_task_counter > 0; }

	inline const std::string& GetName() { return m_name; }

private:
	SafeQueue<PackageBase*> m_package_queue;
	std::string m_name;

	int m_task_counter;
};

}

