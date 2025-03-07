#include "handle_text.h"

#include <sstream>

std::string handle_text::consume_hierarchy_internal(int indent, const generic_hierarchy& node)
{
	std::string indent_str(indent, ' ');
	std::ostringstream ss;
	if (node.has_name())
	{
		ss << indent_str << node.get_name() << "\n";
		node.foreach_child([&](const generic_hierarchy& child) {
			ss << consume_hierarchy_internal(indent + 2, child);
		});
	}
	else
	{
		ss << indent_str << "[\n";
		node.foreach_child([&](const generic_hierarchy& child) {
			ss << consume_hierarchy_internal(indent + 2, child);
		});
		ss << indent_str << "]\n";
	}

	return ss.str();
}
