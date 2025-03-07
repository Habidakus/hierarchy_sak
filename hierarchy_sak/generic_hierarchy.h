#pragma once

#include <optional>
#include <string>
#include <vector>
#include <functional>
#include <memory>

class generic_hierarchy
{
private:
	std::optional<std::string> m_value;
	std::vector<std::shared_ptr<generic_hierarchy>> m_children;

public:
	generic_hierarchy(std::string_view str) : m_value(str) {}
	generic_hierarchy(const char* value) : m_value(value) {}
	generic_hierarchy() {}

	void AddChild(generic_hierarchy* node);
	void foreach_child(std::function<void(const generic_hierarchy&)> func) const;
	inline bool has_children() const { return !m_children.empty(); }
	inline bool has_single_child() const { return m_children.size() == 1; }
	const generic_hierarchy& get_only_child() const;

	//bool is_unnamed_array() const {
	//	if (!m_value.has_value())
	//	{
	//		if (!m_children.empty())
	//		{
	//			return true;
	//		}
	//	}

	//	return false;
	//}

	//bool all_children_unnamed_parents() const {
	//	if (m_children.empty())
	//		return false;

	//	for (auto& child : m_children)
	//	{
	//		if (child.has_name())
	//			return false;
	//		if (child.m_children.empty())
	//			return false;
	//	}

	//	return true;
	//}

	//bool has_single_value() const
	//{
	//	if (m_children.size() == 1)
	//	{
	//		if (m_children[0].m_children.empty())
	//		{
	//			assert(m_children[0].m_value.has_value());
	//			return true;
	//		}
	//	}

	//	return false;
	//}

	//std::string get_single_child_value() const
	//{
	//	return m_children[0].m_value.value();
	//}

	inline bool has_name() const
	{
		return m_value.has_value();
	}

	inline const char* get_name() const
	{
		return m_value->c_str();
	}
};

class IConsumeHierarchy
{
public:
	virtual std::string consume_hierarchy(const generic_hierarchy& hierarchy) = 0;
};

class IProduceHierarchy
{
public:
	virtual generic_hierarchy* parse_hierarchy(std::string_view json_text) = 0;
};