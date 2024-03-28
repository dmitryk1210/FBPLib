#pragma once

#include <chrono>
#include <vector>
#include <mutex>

namespace fbp {

struct DataChunk {
	size_t threadId;
	std::string taskName;
	
	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point stop;
};
class DataCollectorInstance {
	std::vector<DataChunk> m_dataChunks;

public:
	void SubmitDataChunk(const DataChunk& chunk) {
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
};

}
