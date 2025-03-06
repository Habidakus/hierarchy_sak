#pragma once

#include <optional>
#include <string>
#include <vector>
#include <functional>
#include <memory>

class generic_heirarchy
{
private:
	std::optional<std::string> m_value;
	std::vector<std::shared_ptr<generic_heirarchy>> m_children;

public:
	generic_heirarchy(std::string_view str) : m_value(str) {}
	generic_heirarchy(const char* value) : m_value(value) {}
	generic_heirarchy() {}

	void AddChild(generic_heirarchy* node);
	void foreach_child(std::function<void(const generic_heirarchy&)> func) const;
	inline bool has_children() const { return !m_children.empty(); }
	inline bool has_single_child() const { return m_children.size() == 1; }
	const generic_heirarchy& get_only_child() const;

	//~generic_heirarchy() {
	//	for (auto& child : m_children)
	//	{
	//		delete& child;
	//	}
	//	m_children.clear();
	//}

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

	//generic_heirarchy(const generic_heirarchy& other) : m_value(other.m_value) {}
	//generic_heirarchy(const generic_heirarchy&& other) : m_value(std::move(other.m_value)) {}

	inline bool has_name() const
	{
		return m_value.has_value();
	}

	inline const char* get_name() const
	{
		return m_value->c_str();
	}
};

class IConsumeHeirarchy
{
public:
	virtual std::string consume_heirarchy(const generic_heirarchy& heirarchy) = 0;
};

class IProduceHeirarchy
{
public:
	virtual generic_heirarchy* produce_heirarchy(std::string_view json_text) = 0;
};