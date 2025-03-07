#include "generic_hierarchy.h"

#include <cassert>

void generic_hierarchy::AddChild(generic_hierarchy* node)
{
	assert(node != nullptr);
	m_children.push_back(std::make_shared<generic_hierarchy>(*node));
}

void generic_hierarchy::foreach_child(std::function<void(const generic_hierarchy&)> func) const
{
	for (const auto& child : m_children)
	{
		func(*child);
	}
}

const generic_hierarchy& generic_hierarchy::get_only_child() const
{
	std::shared_ptr<generic_hierarchy> child = m_children[0];
	return *(child.get());
}
