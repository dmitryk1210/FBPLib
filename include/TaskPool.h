#pragma once

#include "Task.h"

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
		//if (tED.task->isFinalized() || tED.task->isFinalizeInProcess()) return -1;
		int size = tED.task->getAvaitingPackagesCount();
		if (size <= 0) return 0;
		double result = 1.0;
		if (tED.wasStackEmptied) result /= 4;
		if (size < MIN_AVAITING_PACKAGES_COUNT) result /= 4;
		result *= size;
		result /= MAX(
			MAX(tED.processedPackages * 1.0 / tED.ticks, 1.0) * tED.workingThreads,
			1.0
		);
		return result;
	}

	std::vector<TaskExecutionData> m_taskExecutionDatas;


public:
	void  AddTask    (const std::string&, Task*);
	Task* GetNextTask(Task* pCurTask = nullptr, uint32_t processedPackages = 0, bool wasStackEmptied = false);
};
}
