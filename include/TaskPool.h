#pragma once

#include "Task.h"

#define MIN(a, b) a < b ? a : b
#define MAX(a, b) a > b ? a : b

namespace fbp {
class TaskPool {
private:
	const int MIN_AVAITING_PACKAGES_COUNT = 32;
	struct TaskExecutionData {
		Task*       task              = nullptr;
		std::string name              = "";
		uint32_t    processedPackages = 0;
		uint16_t    ticks             = 0;
		uint16_t    workingThreads    = 0;
		bool        wasStackEmptied   = false;
	};

	double CalculatePriority(TaskExecutionData& tED) {
		int size = tED.task->GetAvaitingPackagesCount();
		if (size <= 0) return 0;
		double result = 1.0;
		if (tED.wasStackEmptied) result /= 64;
		if (size < MIN_AVAITING_PACKAGES_COUNT) result /= 64;
		
		double speed = MAX(tED.processedPackages * 1.0 / tED.ticks, 0.0);
		double totalSpeed = speed * tED.workingThreads;
		double estimate = MIN(size / totalSpeed, INT_MAX);
		result *= estimate;
		return result;
	}

	std::vector<TaskExecutionData> m_taskExecutionDatas;


public:
	void  AddTask    (const std::string&, Task*);
	Task* GetNextTask(Task* pCurTask = nullptr, uint32_t processedPackages = 0, bool wasStackEmptied = false);
};
}