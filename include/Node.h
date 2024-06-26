// Node.h
//

#pragma once

#include "fbpGlobalDefines.h"

#include <iostream>
#include <string>


#define LOCK_FREE_QUEUE

#ifdef LOCK_FREE_QUEUE
#include "concurrentqueue/concurrentqueue.h"
#else
#include "SafeQueue/SafeQueue.hpp"
#endif // LOCK_FREE_QUEUE

#include "Event.h"
#include "PackageBase.h"


namespace fbp {
class Executor;
class Task;

class Node
{
public:
	Node(const std::string& name) : m_name(name) {}

	inline const std::string& GetName() { return m_name; }

	inline void Push(std::unique_ptr<PackageBase> ppackage)
	{
		if (ppackage->IsLast()) {
			OnGetLast.notify();
			return;
		}
#ifdef LOCK_FREE_QUEUE
		m_package_queue.enqueue(std::move(ppackage));
#else
		m_package_queue.Produce(std::move(ppackage));
#endif // LOCK_FREE_QUEUE
	}
	inline std::unique_ptr<PackageBase> Pop()
	{
		std::unique_ptr<PackageBase> ppackage;
#ifdef LOCK_FREE_QUEUE
		bool isSuccess = m_package_queue.try_dequeue(ppackage);
#else
		bool isSuccess = m_package_queue.Consume(ppackage);
#endif // LOCK_FREE_QUEUE
		return ppackage;
	}

	inline int CountApprox() { 
#ifdef LOCK_FREE_QUEUE
		return m_package_queue.size_approx();
#else
		return m_package_queue.Size(); 
#endif // LOCK_FREE_QUEUE
	}

private:
#ifdef LOCK_FREE_QUEUE
	moodycamel::ConcurrentQueue<std::unique_ptr<PackageBase>> m_package_queue;
#else
	SafeQueue<std::unique_ptr<PackageBase>> m_package_queue;
#endif // LOCK_FREE_QUEUE
	std::string m_name;

	Event<> OnGetLast;

	friend class Task;
	friend class Executor;
};

}

