#pragma once

#ifdef FBP_ENABLE_DATA_COLLECTOR

#include <chrono>
#include <vector>
#include <mutex>

#include <ostream>

namespace fbp {

struct DataChunk {
	size_t threadId;
	std::string taskName;
	
	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point stop;
};
class DataCollectorInstance {
	std::vector<DataChunk> m_dataChunks;

	void PrintCollectedData(std::ostream& out) const;

	friend class DataCollector;

public:
	void SubmitDataChunk(const DataChunk& chunk) {
		m_dataChunks.push_back(chunk);
	}
	void SubmitDataChunk(DataChunk&& chunk) {
		m_dataChunks.push_back(chunk);
	}
};

class DataCollector {
private:
	std::vector<DataCollectorInstance>    m_instances;
	std::mutex                            m_getInstanceMtx;
	std::chrono::steady_clock::time_point m_startGlobal;

public:
	void SetStartTimePoint(const std::chrono::steady_clock::time_point& timePoint) { m_startGlobal = timePoint; }
	DataCollectorInstance& CreateInstance();
	void PrintCollectedData(std::ostream& out) const;
	
};

}
#endif // FBP_ENABLE_DATA_COLLECTOR
