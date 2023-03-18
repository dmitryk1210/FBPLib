#pragma once


#include <map>
#include <string>

#include "PackageBase.h"
#include "Node.h"
#include "Task.h"


namespace fbp {
class TaskManager
{
public:
	TaskManager();
	bool addNode(const std::string& node_name);
	bool deleteNode(const std::string& node_name);
	bool addTask(const std::string& task_name, Task& task);
	bool changeTask(const std::string& task_name, Task& task);

	inline bool connect(const std::string& init_node_name, const std::string& task_name, const std::string& result_node_list) {
		return connect(init_node_name, task_name, { result_node_list });
	}
	bool connect(const std::string& init_node_name, const std::string& task_name, const std::initializer_list<std::string>& result_nodes_list);
private:
	std::map<std::string, fbp::Node*> m_map_node;
	std::map<std::string, fbp::Task*> m_map_task;

	Node* getpNode(const std::string& node_name);
	Task* getpTask(const std::string& task_name);
};

}

