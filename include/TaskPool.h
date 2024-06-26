// TaskPool.h
//

#pragma once

#include "fbpGlobalDefines.h"

#include <atomic>
#include <deque>

#include "Task.h"

#define MIN(a, b) a < b ? a : b
#define MAX(a, b) a > b ? a : b

namespace fbp {
class TaskPool {
private:
	const int MIN_AVAITING_PACKAGES_COUNT = 32;
	struct TaskExecutionData {
		Task*                 task              = nullptr;
		std::string           name              = "";
		std::atomic<uint32_t> processedPackages = 0;
		std::atomic<uint16_t> ticks             = 0;
		std::atomic<uint16_t> workingThreads    = 0;
		std::atomic<bool>     wasStackEmptied   = false;
	};

	double CalculatePriority(TaskExecutionData& tED, bool isCurrentTask) {
		int size = tED.task->GetAvaitingPackagesCountApprox();
		if (size <= 0) return 0;
		if (!isCurrentTask && tED.task->GetThreadsLimit() <= tED.workingThreads) return 0;
		double result = 1.0;
		if (tED.wasStackEmptied) result /= 64;
		if (size < MIN_AVAITING_PACKAGES_COUNT) result /= 64;
		
		double speed = MAX(tED.processedPackages * 1.0 / tED.ticks, 0.0);
		double totalSpeed = speed * (tED.workingThreads - isCurrentTask);
		double estimate = MIN(size / totalSpeed, INT_MAX);
		result *= estimate;
		return result;
	}

	std::deque<TaskExecutionData> m_taskExecutionDatas;


public:
	void  AddTask    (const std::string&, Task*);
	Task* GetNextTask(Task* pCurTask = nullptr, uint32_t processedPackages = 0, bool wasStackEmptied = false);
	bool  IsAllFinished();
};
}
