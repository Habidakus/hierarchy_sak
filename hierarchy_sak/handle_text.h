#pragma once

#include "generic_hierarchy.h"

class handle_text : IConsumeHierarchy
{
public:
	std::string consume_hierarchy(const generic_hierarchy& hierarchy) override
	{
		return consume_hierarchy_internal(0, hierarchy);
	}

private:
	std::string consume_hierarchy_internal(int indent, const generic_hierarchy& node);
};