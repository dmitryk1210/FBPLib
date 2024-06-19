#pragma once

#include <iostream>
#include <string>


#define LOCK_FREE_QUEUE

#ifdef LOCK_FREE_QUEUE
#include "concurrentqueue/concurrentqueue.h"
#else
#include "SafeQueue/SafeQueue.hpp"
#endif // LOCK_FREE_QUEUE

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
#ifdef LOCK_FREE_QUEUE
		m_package_queue.enqueue(std::move(ppackage));
#else
		m_package_queue.Produce(std::move(ppackage));
#endif // LOCK_FREE_QUEUE
	}
	inline PackageBase* Pop()
	{
		PackageBase* ppackage;
#ifdef LOCK_FREE_QUEUE
		bool isSuccess = m_package_queue.try_dequeue(ppackage);
#else
		bool isSuccess = m_package_queue.Consume(ppackage);
#endif // LOCK_FREE_QUEUE
		return isSuccess ? ppackage : nullptr;
	}

	inline int Count() { 
#ifdef LOCK_FREE_QUEUE
		return m_package_queue.size_approx();
#else
		return m_package_queue.Size(); 
#endif // LOCK_FREE_QUEUE
	}

	inline void HandleTaskAttached() { ++m_task_counter; }
	inline void HandleTaskDetached() { m_task_counter = std::max(0, m_task_counter - 1); }

	inline bool IsActive() const { return m_task_counter > 0; }

	inline const std::string& GetName() { return m_name; }

private:
#ifdef LOCK_FREE_QUEUE
	moodycamel::ConcurrentQueue<PackageBase*> m_package_queue;
#else
	SafeQueue<PackageBase*> m_package_queue;
#endif // LOCK_FREE_QUEUE
	std::string m_name;

	int m_task_counter;
};

}

