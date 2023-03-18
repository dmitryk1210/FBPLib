#include "TaskManager.h"


namespace fbp {
TaskManager::TaskManager()
	: m_map_node()
	, m_map_task()
{
}

bool TaskManager::addNode(const std::string& node_name) 
{
	if (m_map_node.count(node_name) > 0) return false;
	m_map_node[node_name] = new Node(node_name);
	return true;
}

bool TaskManager::deleteNode(const std::string& node_name) 
{
	auto itNode = m_map_node.find(node_name);
	if (itNode == m_map_node.end()) return false;

	delete itNode->second;
	m_map_node.erase(itNode);
	return true;
}

bool TaskManager::addTask(const std::string& task_name, Task& task) 
{
	if (m_map_task.count(task_name) > 0) return false;
	m_map_task[task_name] = &task;
	return true;
}

bool TaskManager::changeTask(const std::string& task_name, Task& task) 
{
	return false;
}

bool TaskManager::connect(const std::string& init_node_name, const std::string& task_name, const std::initializer_list<std::string>& result_nodes_list) 
{
	//Node* pInitNode = getpNode(init_node_name);
	//if (!pInitNode) return false;

	//Task* pTask = getpTask(task_name);
	//if (!pTask) return false;

	//pInitNode->m_connected_task = pTask;
	//pTask->m_target_nodes.reserve(result_nodes_list.size());
	//for (auto it = result_nodes_list.begin(); it != result_nodes_list.end(); ++it) {
	//	Node* pNode = getpNode(*it);
	//	if (!pNode) return false;
	//	pTask->m_target_nodes.push_back(pNode);
	//}
	return true;
}

Node* TaskManager::getpNode(const std::string& node_name) 
{
	auto itNode = m_map_node.find(node_name);
	if (itNode == m_map_node.end()) return nullptr;
	return itNode->second;
}

Task* TaskManager::getpTask(const std::string& task_name) 
{
	auto itTask = m_map_task.find(task_name);
	if (itTask == m_map_task.end()) return nullptr;
	return itTask->second;
}

}

