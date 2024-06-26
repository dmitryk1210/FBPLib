// DataCollector.cpp
//

#include "pch.h"

#include "fbpGlobalDefines.h"

#ifdef FBP_ENABLE_DATA_COLLECTOR

#include "DataCollector.h"
#include "DataCollectorConstants.h"


namespace fbp {

void DataCollectorInstance::PrintCollectedData(std::ostream& out) const {
	out << DC_INSTANCE_BEGIN << DC_ENDLINE;
	for (const auto& chunk : m_dataChunks) {
		out << chunk.threadId << DC_SEPARATOR
			<< chunk.taskName << DC_SEPARATOR
			<< chunk.start.time_since_epoch().count() << DC_SEPARATOR
			<< chunk.stop.time_since_epoch().count()  << DC_ENDLINE;
	}
	out << DC_INSTANCE_END << DC_ENDLINE;
}


DataCollectorInstance& DataCollector::CreateInstance() {
	std::lock_guard<std::mutex> guard(m_getInstanceMtx);
	m_instances.reserve(32);
	m_instances.push_back(DataCollectorInstance());
	return m_instances.back();
}


void DataCollector::PrintCollectedData(std::ostream& out) const {
	out << DC_START_CAPTURE_TIME << DC_SEPARATOR 
		<< m_startGlobal.time_since_epoch().count() << DC_ENDLINE;
	out << DC_STOP_CAPTURE_TIME  << DC_SEPARATOR 
		<< std::chrono::high_resolution_clock::now().time_since_epoch().count() << DC_ENDLINE;
	out << DC_INSTANCES_COUNT << DC_SEPARATOR << m_instances.size() << DC_ENDLINE;
	out << DC_INSTANCES_SECTION_BEGIN << DC_ENDLINE;
	for (const auto& instance : m_instances) {
		instance.PrintCollectedData(out);
	}
	out << DC_INSTANCES_SECTION_END << DC_ENDLINE;
}

}
#endif // FBP_ENABLE_DATA_COLLECTOR
