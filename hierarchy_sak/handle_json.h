#pragma once

#include "generic_heriarchy.h"

#include <rapidjson/document.h>

class handle_json : IConsumeHeirarchy, IProduceHeirarchy
{
public:
	handle_json() {}
	generic_heirarchy* produce_heirarchy(std::string_view json_text) override;
	std::string consume_heirarchy(const generic_heirarchy& root) override;

private:
	generic_heirarchy* create_heirarchy(const char* name, rapidjson::Value::ConstMemberIterator begin, rapidjson::Value::ConstMemberIterator end);
	generic_heirarchy* create_heirarchy(const char* name, rapidjson::Value::ConstValueIterator begin, rapidjson::Value::ConstValueIterator end);
	generic_heirarchy* create_heirarchy(const char* name, const rapidjson::Value* value);

	void populate_children(generic_heirarchy* root, rapidjson::Value::ConstMemberIterator begin, rapidjson::Value::ConstMemberIterator end);
	void populate_children(generic_heirarchy* root, rapidjson::Value::ConstValueIterator begin, rapidjson::Value::ConstValueIterator end);

	void consume_into_object(rapidjson::Value& object, const std::vector<generic_heirarchy>& children, rapidjson::Document& doc);
	void consume_into_array(rapidjson::Value& array, const std::vector<generic_heirarchy>& children, rapidjson::Document& doc);

	bool are_any_children_parents(const std::vector<generic_heirarchy>& children);
	bool are_all_children_named(const std::vector<generic_heirarchy>& children);
	bool are_no_children_named(const std::vector<generic_heirarchy>& children);

	rapidjson::Value consume_into_attribute_value(const std::vector<generic_heirarchy>& children, rapidjson::Document& doc);
	rapidjson::Value consume_into_attribute_value(const generic_heirarchy& node, rapidjson::Document& doc);
};