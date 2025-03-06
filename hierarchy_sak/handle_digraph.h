#pragma once

#include "generic_heriarchy.h"
#include <map>

class handle_digraph : IConsumeHeirarchy
{
private:
	uint32_t m_next_node_id = 0;
	std::map<const generic_heirarchy*, std::string> m_node_names;
public:
	std::string consume_heirarchy(const generic_heirarchy& heirarchy) override;

private:
	std::string get_node_graph_name(const generic_heirarchy& node);
	std::string generate_edges(const generic_heirarchy& node);
	std::string dump_node_names() const;
	std::string convert_node_name_to_graph_node(const generic_heirarchy& node) const;
	std::string get_a_viable_child_node_name(const generic_heirarchy& child);
};