#include "pch.h"

#include "DataCollector.h"

namespace fbp {
DataCollectorInstance& DataCollector::CreateInstance() {
	std::lock_guard<std::mutex> guard(m_getInstanceMtx);
	m_instances.reserve(32);
	m_instances.push_back(DataCollectorInstance());
	return m_instances.back();
}
}
