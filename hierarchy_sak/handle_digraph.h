#pragma once

#include "generic_hierarchy.h"
#include <map>

class handle_digraph : IConsumeHierarchy
{
private:
	uint32_t m_next_node_id = 0;
	std::map<const generic_hierarchy*, std::string> m_node_names;
public:
	std::string consume_hierarchy(const generic_hierarchy& hierarchy) override;

private:
	std::string get_node_graph_name(const generic_hierarchy& node);
	std::string generate_edges(const generic_hierarchy& node);
	std::string dump_node_names() const;
	std::string convert_node_name_to_graph_node(const generic_hierarchy& node) const;
	std::string get_a_viable_child_node_name(const generic_hierarchy& child);
};