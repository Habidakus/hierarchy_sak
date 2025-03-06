#include "generic_heriarchy.h"

#include <cassert>

void generic_heirarchy::AddChild(generic_heirarchy* node)
{
	assert(node != nullptr);
	m_children.push_back(std::make_shared<generic_heirarchy>(*node));
}

void generic_heirarchy::foreach_child(std::function<void(const generic_heirarchy&)> func) const
{
	for (const auto& child : m_children)
	{
		func(*child);
	}
}

const generic_heirarchy& generic_heirarchy::get_only_child() const
{
	std::shared_ptr<generic_heirarchy> child = m_children[0];
	return *(child.get());
}
