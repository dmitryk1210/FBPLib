#include "pch.h"

#include "Node.h"


namespace fbp {
Node::Node(const std::string& name)
	: m_name(name)
	, m_package_queue()
{
}

}
