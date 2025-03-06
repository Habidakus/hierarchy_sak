#include "handle_text.h"

#include <sstream>

std::string handle_text::consume_heirarchy_internal(int indent, const generic_heirarchy& node)
{
	std::string indent_str(indent, ' ');
	std::ostringstream ss;
	if (node.has_name())
	{
		ss << indent_str << node.get_name() << "\n";
		for (const generic_heirarchy& child : node.m_children)
		{
			ss << consume_heirarchy_internal(indent + 2, child);
		}
	}
	else
	{
		ss << indent_str << "[\n";
		for (const generic_heirarchy& child : node.m_children)
		{
			ss << consume_heirarchy_internal(indent + 2, child);
		}

		ss << indent_str << "]\n";
	}

	return ss.str();
}
