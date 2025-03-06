#include "handle_digraph.h"

#include <sstream>

std::string handle_digraph::consume_heirarchy(const generic_heirarchy& heirarchy)
{
	std::ostringstream ss;
	ss << "digraph G {\n";
	ss << "  layout=dot;\n";
	ss << "  compound=true;\n";
	ss << generate_edges(heirarchy);
	ss << dump_node_names();
	ss << "}\n";
	return ss.str();
}

std::string handle_digraph::generate_edges(const generic_heirarchy& node)
{
	std::ostringstream ss;
	std::string graph_node_name = get_node_graph_name(node);

	if (!node.has_name())
	{
		ss << "  subgraph cluster_" << graph_node_name << " {\n";
	}

	if (node.has_name())
	{
		node.foreach_child([&ss, graph_node_name, this](const generic_heirarchy& child) {
			std::string child_graph_node_name = get_node_graph_name(child);
			if (child.has_name())
			{
				ss << "  " << graph_node_name << " -> " << child_graph_node_name;
			}
			else
			{
				ss << "  " << graph_node_name << " -> ";
				ss << get_a_viable_child_node_name(child);
				ss << " [lhead=\"cluster_" << child_graph_node_name << "\"]";
			}
			ss << ";\n";
		});
	}

	node.foreach_child([&ss, this](const generic_heirarchy& child) {
		ss << generate_edges(child);
	});

	if (!node.has_name())
	{
		ss << "  }\n";
	}

	return ss.str();
}

std::string handle_digraph::get_a_viable_child_node_name(const generic_heirarchy& node)
{
	if (node.has_name())
	{
		return get_node_graph_name(node);
	}

	node.foreach_child([this](const generic_heirarchy& child) {
		std::string child_graph_node_name = get_a_viable_child_node_name(child);
		if (!child_graph_node_name.empty())
		{
			return child_graph_node_name;
		}
	});

	return "";
}

std::string handle_digraph::dump_node_names() const
{
	std::ostringstream ss;
	for (auto& entry : m_node_names)
	{
		if (entry.first->has_name())
		{
			ss << "  " << entry.second << " [label=\"";
			std::string entry_name = std::string(entry.first->get_name());
			if (entry_name.find('\n') != std::string::npos)
			{
				// # TODO: FIX THIS
				entry_name = "MULTIPLE LINES";
			};
			ss << entry_name;
			if (!entry_name.empty() && entry_name.back() == '\\')
			{
				ss << " ";
			}

			ss << "\"];\n";
		}
		else
		{
			//ss << "  " << entry.second << " [shape=square, label=\"\"];\n";
		}
	}

	return ss.str();
}

std::string handle_digraph::get_node_graph_name(const generic_heirarchy& node)
{
	if (m_node_names.find(&node) == m_node_names.end())
	{
		std::string node_name_as_graph_node = convert_node_name_to_graph_node(node);
		m_node_names[&node] = node_name_as_graph_node + "_" + std::to_string(++m_next_node_id);
	}

	return m_node_names[&node];
}

std::string handle_digraph::convert_node_name_to_graph_node(const generic_heirarchy& node) const
{
	if (node.has_name())
	{
		std::string node_name(node.get_name());
		if (node_name.empty())
		{
			return "empty";
		}

		bool first = true;
		std::ostringstream ss;
		for (char c : node_name)
		{
			if (isalnum(c))
			{
				if (first && isdigit(c))
				{
					ss << "num_";

					first = false;
				}

				if (!ispunct(c))
				{
					ss << c;

					first = false;
				}
			}
			else if (isspace(c) || c == '_')
			{
				if (first)
				{
					ss << "space";
				}
				ss << '_';

				first = false;
			}
		}

		return ss.str();
	}
	else
	{
		return "node";
	}
}

