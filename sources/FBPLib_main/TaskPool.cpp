// TaskPool.cpp
//

#include "pch.h"

#include "Executor.h"
#include "TaskPool.h"

namespace fbp {

void TaskPool::AddTask(const std::string& name, Task* pTask) {
	m_taskExecutionDatas.emplace_back();
	m_taskExecutionDatas.back().name = name;
	m_taskExecutionDatas.back().task = pTask;
}

Task* TaskPool::GetNextTask(Task* pCurTask, uint32_t processedPackages, bool wasStackEmptied) {
	std::vector<double> priorities;
	priorities.resize(m_taskExecutionDatas.size());
	int    curTaskIdx      = -1;
	double curTaskPriority = 0.0;

	int    maxTaskIdx      = -1;
	double maxTaskPriority = 0.0;

	for (int i = 0; i < m_taskExecutionDatas.size(); ++i) {
		bool isCurrentTask = m_taskExecutionDatas[i].task == pCurTask;
		priorities[i] = CalculatePriority(m_taskExecutionDatas[i], isCurrentTask);
		if (isCurrentTask) {
			curTaskIdx = i;
			curTaskPriority = priorities[i];
			m_taskExecutionDatas[i].ticks.fetch_add(1, std::memory_order_relaxed);
			m_taskExecutionDatas[i].processedPackages.fetch_add(processedPackages, std::memory_order_relaxed);
			m_taskExecutionDatas[i].wasStackEmptied.store(wasStackEmptied);
		}
		if (priorities[i] > maxTaskPriority) {
			maxTaskIdx = i;
			maxTaskPriority = priorities[i];
		}
	}

	if (curTaskPriority / maxTaskPriority > 0.1 && curTaskIdx >= 0) {
		return pCurTask;
	}
	if (curTaskIdx >= 0) {
		m_taskExecutionDatas[curTaskIdx].workingThreads.fetch_add(-1);
	}
	if (maxTaskIdx >= 0) {
		m_taskExecutionDatas[maxTaskIdx].workingThreads.fetch_add(1);
		return m_taskExecutionDatas[maxTaskIdx].task;
	}
	return nullptr;
}

bool TaskPool::IsAllFinished() {
	int accumulate = 0;
	for (int i = 0; i < m_taskExecutionDatas.size(); ++i) {
		accumulate += m_taskExecutionDatas[i].workingThreads;
	}
	return !accumulate;
}

}
