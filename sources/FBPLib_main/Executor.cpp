#include "pch.h"
#include "Executor.h"


namespace fbp {

void Executor::threadExecute(ThreadData& threadData)
{
	threadData.pTask->doTask();
}

void Executor::execute()
{
	int numThreads = 0;
	for (auto it = m_tasks.cbegin(); it != m_tasks.cend(); ++it)
	{
		numThreads += it->second.getNumThreads();
	}

	m_threads.clear();
	m_threadDatas.clear();
	m_threads.reserve(numThreads);
	m_threadDatas.reserve(numThreads);

	int index = 0;
	for (auto it = m_tasks.begin(); it != m_tasks.end(); ++it)
	{
		for (int i = 0; i < it->second.getNumThreads(); ++i)
		{
			m_threadDatas.push_back(ThreadData());
			m_threadDatas[index].id = index;
			m_threadDatas[index].pTask = &(it->second);
			m_threads.push_back(std::thread([this, index]() { this->threadExecute(this->m_threadDatas[index]); }));
			++index;
		}
	}

}
}

