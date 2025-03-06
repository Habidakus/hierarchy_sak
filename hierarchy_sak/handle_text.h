#pragma once

#include "generic_heriarchy.h"

class handle_text : IConsumeHeirarchy
{
public:
	std::string consume_heirarchy(const generic_heirarchy& heirarchy) override
	{
		return consume_heirarchy_internal(0, heirarchy);
	}

private:
	std::string consume_heirarchy_internal(int indent, const generic_heirarchy& node);
};