#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#include "handle_json.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <sstream>
#include <cassert>

generic_heirarchy* handle_json::produce_heirarchy(std::string_view json_text)
{
	generic_heirarchy* root = new generic_heirarchy();
	rapidjson::Document doc;
	doc.Parse(json_text.data());
	populate_children(root, doc.GetObject().begin(), doc.GetObject().end());
	return root;
}

std::string handle_json::consume_heirarchy(const generic_heirarchy& root)
{
	rapidjson::Document doc;
	consume_children_into_object(doc.SetObject(), root, doc);
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);
	return buffer.GetString();
}

generic_heirarchy* handle_json::create_heirarchy(const char* name, rapidjson::Value::ConstMemberIterator begin, rapidjson::Value::ConstMemberIterator end)
{
	generic_heirarchy* root = new generic_heirarchy(name);
	populate_children(root, begin, end);
	return root;
}

generic_heirarchy* handle_json::create_heirarchy(const char* name, rapidjson::Value::ConstValueIterator begin, rapidjson::Value::ConstValueIterator end)
{
	generic_heirarchy* root = new generic_heirarchy(name);
	populate_children(root, begin, end);
	return root;
}

generic_heirarchy* handle_json::create_heirarchy(const char* name, const rapidjson::Value* value)
{
	generic_heirarchy* root = new generic_heirarchy(name);
	if (value->IsObject())
	{
		populate_children(root, value->MemberBegin(), value->MemberEnd());
	}
	else if (value->IsArray())
	{
		populate_children(root, value->Begin(), value->End());
	}
	else if (value->IsString())
	{
		root->AddChild(new generic_heirarchy(value->GetString()));
	}
	else
	{
		std::ostringstream ss;
		if (value->IsBool())
		{
			ss << (value->GetBool() ? "true" : "false");
		}
		else if (value->IsInt())
		{
			ss << value->GetInt();
		}
		else if (value->IsDouble())
		{
			ss << value->GetDouble();
		}
		else if (value->IsFalse())
		{
			ss << "false";
		}
		else if (value->IsTrue())
		{
			ss << "true";
		}
		else if (value->IsUint())
		{
			ss << value->GetUint();
		}
		else if (value->IsInt64())
		{
			ss << value->GetInt64();
		}
		else if (value->IsUint64())
		{
			ss << value->GetUint64();
		}
		else if (value->IsNull())
		{
			ss << "null";
		}
		else
		{
			ss << "???";
		}

		root->AddChild(new generic_heirarchy(ss.str()));
	}

	return root;
}

void handle_json::populate_children(generic_heirarchy* root, rapidjson::Value::ConstMemberIterator begin, rapidjson::Value::ConstMemberIterator end)
{
	for (auto iter = begin; iter != end; ++iter)
	{
		const char* iter_name = iter->name.GetString();
		if (iter->value.IsObject())
		{
			generic_heirarchy* node = create_heirarchy(iter_name, iter->value.MemberBegin(), iter->value.MemberEnd());
			root->AddChild(node);
		}
		else if (iter->value.IsArray())
		{
			generic_heirarchy* node = new generic_heirarchy(iter_name);
			populate_children(node, iter->value.Begin(), iter->value.End());
			root->AddChild(node);
		}
		else
		{
			const rapidjson::Value* value = &iter->value;
			generic_heirarchy* node = create_heirarchy(iter_name, value);
			root->AddChild(node);
		}
	}
}

void handle_json::populate_children(generic_heirarchy* root, rapidjson::Value::ConstValueIterator begin, rapidjson::Value::ConstValueIterator end)
{
	for (auto iter = begin; iter != end; ++iter)
	{
		if (iter->IsObject())
		{
			generic_heirarchy* node = new generic_heirarchy();
			for (auto& member : iter->GetObject())
			{
				node->AddChild(create_heirarchy(member.name.GetString(), &member.value));
			}

			root->AddChild(node);
		}
		else if (iter->IsArray())
		{
			for (auto el = iter->Begin(); el != iter->End(); ++el)
			{
				if (el->IsObject())
				{
					generic_heirarchy* node = new generic_heirarchy();
					populate_children(node, iter->GetObject().MemberBegin(), iter->GetObject().MemberEnd());
					root->AddChild(node);
				}
				else
				{
					generic_heirarchy* node = new generic_heirarchy(iter->GetObject().MemberBegin()->name.GetString());
					root->AddChild(node);
				}
			}
		}
		else if (iter->GetString())
		{
			generic_heirarchy* node = new generic_heirarchy(iter->GetString());
			root->AddChild(node);
		}
		else
		{
			generic_heirarchy* node = new generic_heirarchy("???");
			root->AddChild(node);
		}
	}
}

void handle_json::consume_children_into_object(rapidjson::Value& object, const generic_heirarchy& parent, rapidjson::Document& doc)
{
	parent.foreach_child([&object, &doc, this](const generic_heirarchy& member_value_pair) {
		assert(member_value_pair.has_name() || member_value_pair.has_children());
		const char* name = member_value_pair.get_name();
		rapidjson::Value value = consume_children_into_attribute_value(member_value_pair, doc);
		rapidjson::GenericValue<rapidjson::UTF8<> > name_value(name, doc.GetAllocator());
		object.AddMember(name_value, value, doc.GetAllocator());
	});
}

void handle_json::consume_children_into_array(rapidjson::Value& array, const generic_heirarchy& parent, rapidjson::Document& doc)
{
	parent.foreach_child([&array, &doc, this](const generic_heirarchy& child) {
		rapidjson::Value value = consume_into_attribute_value(child, doc);
		array.PushBack(value, doc.GetAllocator());
	});
}

bool handle_json::are_any_children_parents(const generic_heirarchy& parent)
{
	bool ret_val = false;
	parent.foreach_child([&ret_val](const generic_heirarchy& child) {
		if (!ret_val)
		{
			ret_val = child.has_children();
		}
	});

	return ret_val;
}

bool handle_json::are_all_children_named(const generic_heirarchy& parent)
{
	bool ret_val = true;
	parent.foreach_child([&ret_val](const generic_heirarchy& child) {
		if (ret_val)
		{
			ret_val = child.has_name();
		}
	});

	return ret_val;
}

bool handle_json::are_no_children_named(const generic_heirarchy& parent)
{
	bool ret_val = true;
	parent.foreach_child([&ret_val](const generic_heirarchy& child) {
		if (ret_val)
		{
			ret_val = !child.has_name();
		}
	});

	return ret_val;
}

rapidjson::Value handle_json::consume_children_into_attribute_value(const generic_heirarchy& parent, rapidjson::Document& doc)
{
	rapidjson::Value value;
	if (!parent.has_children())
	{
		//value.SetNull();
		value.SetObject();
		return value;
	}

	if (!parent.has_single_child())
	{
		if (are_all_children_named(parent))
		{
			if (are_any_children_parents(parent))
			{
				value.SetObject();
				consume_children_into_object(value, parent, doc);
			}
			else
			{
				value.SetArray();
				consume_children_into_array(value, parent, doc);
			}
		}
		else if (are_no_children_named(parent))
		{
			value.SetArray();
			consume_children_into_array(value, parent, doc);
		}
		else
		{
			assert(false);
		}

		return value;
	}

	value = consume_into_attribute_value(parent.get_only_child(), doc);
	return value;
}

rapidjson::Value handle_json::consume_into_attribute_value(const generic_heirarchy& node, rapidjson::Document& doc)
{
	rapidjson::Value value;
	if (!node.has_children())
	{
		std::string name_as_text(node.get_name());

		if (name_as_text == "true")
		{
			value.SetBool(true);
			return value;
		}
		if (name_as_text == "false")
		{
			value.SetBool(false);
			return value;
		}

		char* end;

		uint64_t name_as_uint64 = std::strtoul(name_as_text.c_str(), &end, 10);
		if (end != name_as_text.c_str() && *end == '\0')
		{
			value.SetUint64(name_as_uint64);
			return value;
		}

		int64_t name_as_int64 = std::strtol(name_as_text.c_str(), &end, 10);
		if (end != name_as_text.c_str() && *end == '\0')
		{
			value.SetInt64(name_as_int64);
			return value;
		}

		double name_as_double = std::strtod(name_as_text.c_str(), &end);
		if (end != name_as_text.c_str() && *end == '\0')
		{
			value.SetDouble(name_as_double);
			return value;
		}

		value.SetString(name_as_text.c_str(), doc.GetAllocator());
		return value;
	}

	assert(are_all_children_named(node));
	if (are_any_children_parents(node))
	{
		value.SetObject();
		consume_children_into_object(value, node, doc);
	}
	else
	{
		value.SetArray();
		consume_children_into_array(value, node, doc);
	}

	return value;
}
