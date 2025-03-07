#pragma once

#include "generic_hierarchy.h"

#include <rapidjson/document.h>

class handle_json : IConsumeHierarchy, IProduceHierarchy
{
public:
	handle_json() {}
	generic_hierarchy* parse_hierarchy(std::string_view json_text) override;
	std::string consume_hierarchy(const generic_hierarchy& root) override;

private:
	generic_hierarchy* create_hierarchy(const char* name, rapidjson::Value::ConstMemberIterator begin, rapidjson::Value::ConstMemberIterator end);
	generic_hierarchy* create_hierarchy(const char* name, rapidjson::Value::ConstValueIterator begin, rapidjson::Value::ConstValueIterator end);
	generic_hierarchy* create_hierarchy(const char* name, const rapidjson::Value* value);

	void populate_children(generic_hierarchy* root, rapidjson::Value::ConstMemberIterator begin, rapidjson::Value::ConstMemberIterator end);
	void populate_children(generic_hierarchy* root, rapidjson::Value::ConstValueIterator begin, rapidjson::Value::ConstValueIterator end);

	void consume_children_into_object(rapidjson::Value& object, const generic_hierarchy& parent, rapidjson::Document& doc);
	void consume_children_into_array(rapidjson::Value& array, const generic_hierarchy& parent, rapidjson::Document& doc);

	bool are_any_children_parents(const generic_hierarchy& parent);
	bool are_all_children_named(const generic_hierarchy& parent);
	bool are_no_children_named(const generic_hierarchy& parent);

	rapidjson::Value consume_children_into_attribute_value(const generic_hierarchy& parent, rapidjson::Document& doc);
	rapidjson::Value consume_into_attribute_value(const generic_hierarchy& node, rapidjson::Document& doc);
};