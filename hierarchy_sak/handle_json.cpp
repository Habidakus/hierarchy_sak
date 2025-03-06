#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#include "handle_json.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <sstream>

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
	consume_into_object(doc.SetObject(), root.m_children, doc);
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
		root->m_children.push_back(generic_heirarchy(value->GetString()));
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

		root->m_children.push_back(generic_heirarchy(ss.str()));
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
			root->m_children.push_back(*node);
		}
		else if (iter->value.IsArray())
		{
			generic_heirarchy* node = new generic_heirarchy(iter_name);
			populate_children(node, iter->value.Begin(), iter->value.End());
			root->m_children.push_back(*node);
		}
		else
		{
			const rapidjson::Value* value = &iter->value;
			generic_heirarchy* node = create_heirarchy(iter_name, value);
			root->m_children.push_back(*node);
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
				node->m_children.push_back(*create_heirarchy(member.name.GetString(), &member.value));
			}
			root->m_children.push_back(*node);
		}
		else if (iter->IsArray())
		{
			for (auto el = iter->Begin(); el != iter->End(); ++el)
			{
				if (el->IsObject())
				{
					generic_heirarchy* node = new generic_heirarchy();
					populate_children(node, iter->GetObject().MemberBegin(), iter->GetObject().MemberEnd());
					root->m_children.push_back(*node);
				}
				else
				{
					generic_heirarchy* node = new generic_heirarchy(iter->GetObject().MemberBegin()->name.GetString());
					root->m_children.push_back(*node);
				}
			}
		}
		else if (iter->GetString())
		{
			generic_heirarchy* node = new generic_heirarchy(iter->GetString());
			root->m_children.push_back(*node);
		}
		else
		{
			generic_heirarchy* node = new generic_heirarchy("???");
			root->m_children.push_back(*node);
		}
	}
}

void handle_json::consume_into_object(rapidjson::Value& object, const std::vector<generic_heirarchy>& children, rapidjson::Document& doc)
{
	for (const generic_heirarchy& member_value_pair : children)
	{
		assert(member_value_pair.has_name() || !member_value_pair.m_children.empty());
		const char* name = member_value_pair.get_name();
		rapidjson::Value value = consume_into_attribute_value(member_value_pair.m_children, doc);
		rapidjson::GenericValue<rapidjson::UTF8<> > name_value(name, doc.GetAllocator());
		object.AddMember(name_value, value, doc.GetAllocator());
	}
}

void handle_json::consume_into_array(rapidjson::Value& array, const std::vector<generic_heirarchy>& children, rapidjson::Document& doc)
{
	for (const generic_heirarchy& child : children)
	{
		rapidjson::Value value = consume_into_attribute_value(child, doc);
		array.PushBack(value, doc.GetAllocator());
	}
}

bool handle_json::are_any_children_parents(const std::vector<generic_heirarchy>& children)
{
	for (const generic_heirarchy& child : children)
	{
		if (!child.m_children.empty())
		{
			return true;
		}
	}

	return false;
}

bool handle_json::are_all_children_named(const std::vector<generic_heirarchy>& children)
{
	for (const generic_heirarchy& child : children)
	{
		if (!child.has_name())
		{
			return false;
		}
	}

	return true;
}

bool handle_json::are_no_children_named(const std::vector<generic_heirarchy>& children)
{
	for (const generic_heirarchy& child : children)
	{
		if (child.has_name())
		{
			return false;
		}
	}

	return true;
}

rapidjson::Value handle_json::consume_into_attribute_value(const std::vector<generic_heirarchy>& children, rapidjson::Document& doc)
{
	rapidjson::Value value;
	if (children.empty())
	{
		value.SetNull();
		return value;
	}

	if (children.size() > 1)
	{
		if (are_all_children_named(children))
		{
			if (are_any_children_parents(children))
			{
				value.SetObject();
				consume_into_object(value, children, doc);
			}
			else
			{
				value.SetArray();
				consume_into_array(value, children, doc);
			}
		}
		else if (are_no_children_named(children))
		{
			value.SetArray();
			consume_into_array(value, children, doc);
		}
		else
		{
			assert(false);
		}

		return value;
	}

	value = consume_into_attribute_value(children[0], doc);
	return value;
}

rapidjson::Value handle_json::consume_into_attribute_value(const generic_heirarchy& node, rapidjson::Document& doc)
{
	rapidjson::Value value;
	if (node.m_children.empty())
	{
		value.SetString(node.get_name(), doc.GetAllocator());
		return value;
	}

	assert(are_all_children_named(node.m_children));
	if (are_any_children_parents(node.m_children))
	{
		value.SetObject();
		consume_into_object(value, node.m_children, doc);
	}
	else
	{
		value.SetArray();
		consume_into_array(value, node.m_children, doc);
	}

	return value;
}
