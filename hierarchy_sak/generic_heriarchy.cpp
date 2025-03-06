#include "generic_heriarchy.h"

#include <cassert>

void generic_heirarchy::AddChild(generic_heirarchy* node)
{
	assert(node != nullptr);
	m_children.push_back(*node);
}

void generic_heirarchy::foreach_child(std::function<void(const generic_heirarchy&)> func) const
{
	for (const auto& child : m_children)
	{
		func(child);
	}
}
