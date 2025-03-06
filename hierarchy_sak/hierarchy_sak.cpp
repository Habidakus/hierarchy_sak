// hierarchy_sak.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#include <iostream>
#include <vector>
#include <optional>
#include <rapidjson/document.h>
//#include <format>

#include <sstream>
#include <string>
#include "../packages/tencent.rapidjson.1.1.1/lib/native/include/rapidjson/stringbuffer.h"
#include "../packages/tencent.rapidjson.1.1.1/lib/native/include/rapidjson/writer.h"

class generic_heirarchy
{
private:
	std::optional<std::string> m_value;
public:
	generic_heirarchy(std::string_view str) : m_value(str) { }
	generic_heirarchy(const char* value) : m_value(value) { }
	generic_heirarchy() {}
	//~generic_heirarchy() {
	//	for (auto& child : m_children)
	//	{
	//		delete& child;
	//	}
	//	m_children.clear();
	//}

	bool is_unnamed_array() const {
		if (!m_value.has_value())
		{
			if (!m_children.empty())
			{
				return true;
			}
		}

		return false;
	}

	bool all_children_unnamed_parents() const {
		if (m_children.empty())
			return false;

		for (auto& child : m_children)
		{
			if (child.has_name())
				return false;
			if (child.m_children.empty())
				return false;
		}

		return true;
	}

	bool has_single_value() const
	{
		if (m_children.size() == 1)
		{
			if (m_children[0].m_children.empty())
			{
				assert(m_children[0].m_value.has_value());
				return true;
			}
		}

		return false;
	}

	std::string get_single_child_value() const
	{
		return m_children[0].m_value.value();
	}

	//void dump(std::ostringstream& ss) const {
	//	ss << " ";

	//	if (m_value.has_value())
	//	{
	//		ss << "<" << m_value.value();
	//		if (!m_children.empty())
	//			ss << " :";
	//	}
	//	else
	//	{
	//		ss << "[";
	//	}

	//	for (auto& child : m_children)
	//	{
	//		child.dump(ss);
	//	}

	//	if (m_value.has_value())
	//	{
	//		ss << ">";
	//	}
	//	else
	//	{
	//		ss << "]";
	//	}
	//}

	//generic_heirarchy(const generic_heirarchy& other) : m_value(other.m_value) {}
	//generic_heirarchy(const generic_heirarchy&& other) : m_value(std::move(other.m_value)) {}
	std::vector<generic_heirarchy> m_children;

	bool has_name() const
	{
		return m_value.has_value();
	}

	const char* get_name() const
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

class handle_text : IConsumeHeirarchy
{
public:
	// Inherited via IConsumeHeirarchy
	std::string consume_heirarchy(const generic_heirarchy& heirarchy) override
	{
		return consume_heirarchy_internal(0, heirarchy);
	}

private:
	std::string consume_heirarchy_internal(int indent, const generic_heirarchy& node)
	{
		std::string indent_str(indent, ' ');
		std::ostringstream ss;
		if (node.has_name())
		{
			ss << indent_str << node.get_name() << "\n";
			for (const generic_heirarchy& child : node.m_children)
			{
				ss << consume_heirarchy_internal(indent + 2, child);
			}
		}
		else
		{
			ss << indent_str << "[\n";
			for (const generic_heirarchy& child : node.m_children)
			{
				ss << consume_heirarchy_internal(indent + 2, child);
			}
			ss << indent_str << "]\n";
		}

		return ss.str();
	}
};

class handle_json : IConsumeHeirarchy, IProduceHeirarchy
{
public:
	handle_json() {}
	generic_heirarchy* produce_heirarchy(std::string_view json_text) override
	{
		generic_heirarchy* root = new generic_heirarchy();
		rapidjson::Document doc;
		doc.Parse(json_text.data());
		populate_children(root, doc.GetObject().begin(), doc.GetObject().end());
		return root;
	}
	std::string consume_heirarchy(const generic_heirarchy& root) override
	{
		//rapidjson::Value root_value = create_json(root);
		rapidjson::Document doc;
		consume_into_object(doc.SetObject(), root.m_children, doc);
		//doc.AddMember("root", root_value, doc.GetAllocator());
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		doc.Accept(writer);
		return buffer.GetString();
	}

private:
	generic_heirarchy* create_heirarchy(const char* name, rapidjson::Value::ConstMemberIterator begin, rapidjson::Value::ConstMemberIterator end)
	{
		generic_heirarchy* root = new generic_heirarchy(name);
		populate_children(root, begin, end);
		return root;
	}
	generic_heirarchy* create_heirarchy(const char* name, rapidjson::Value::ConstValueIterator begin, rapidjson::Value::ConstValueIterator end)
	{
		generic_heirarchy* root = new generic_heirarchy(name);
		populate_children(root, begin, end);
		return root;
	}
	generic_heirarchy* create_heirarchy(const char* name, const rapidjson::Value* value)
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
			else
			{
				ss << "???";
			}

			root->m_children.push_back(generic_heirarchy(ss.str()));
		}
		return root;
	}

	void populate_children(generic_heirarchy* root, rapidjson::Value::ConstMemberIterator begin, rapidjson::Value::ConstMemberIterator end)
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
	void populate_children(generic_heirarchy* root, rapidjson::Value::ConstValueIterator begin, rapidjson::Value::ConstValueIterator end)
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

 //   void populate_children(generic_heirarchy* root, nlohmann::json::const_iterator begin, nlohmann::json::const_iterator end)
 //   {
	//	for (auto iter = begin; iter != end; ++iter)
	//	{
	//		nlohmann::json::value_t type = iter->type();
	//		if (type == nlohmann::json::value_t::object)
	//		{
	//			std::cout << "object key:" << iter.key() << std::endl;
	//			generic_heirarchy* node = create_heirarchy(iter.key(), iter->cbegin(), iter->cend());
	//			root->m_children.push_back(*node);
	//		}
	//		else if (iter->is_array())
	//		{
	//			for (auto el : *iter)
	//			{
	//				if (el.is_object())
	//				{
	//					generic_heirarchy* node = create_heirarchy(iter.key(), iter->cbegin(), iter->cend());
	//					root->m_children.push_back(*node);
	//				}
	//				else
	//				{
	//					generic_heirarchy* node = new generic_heirarchy(iter.key());
	//					root->m_children.push_back(*node);
	//				}
	//			}
	//		}
	//		else
	//		{
	//			generic_heirarchy* node = new generic_heirarchy(iter.key());
	//			root->m_children.push_back(*node);
	//		}
	//	}
	//}

	void consume_into_object(rapidjson::Value& object, const std::vector<generic_heirarchy>& children, rapidjson::Document& doc)
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

	void consume_into_array(rapidjson::Value& array, const std::vector<generic_heirarchy>& children, rapidjson::Document& doc)
	{
		for (const generic_heirarchy& child : children)
		{
			rapidjson::Value value = consume_into_attribute_value(child, doc);
			array.PushBack(value, doc.GetAllocator());
		}
	}

	bool are_any_children_parents(const std::vector<generic_heirarchy>& children)
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

	bool are_all_children_named(const std::vector<generic_heirarchy>& children)
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

	bool are_no_children_named(const std::vector<generic_heirarchy>& children)
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

	rapidjson::Value consume_into_attribute_value(const std::vector<generic_heirarchy>& children, rapidjson::Document& doc)
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

	rapidjson::Value consume_into_attribute_value(const generic_heirarchy& node, rapidjson::Document& doc)
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

	//void add_children_deprecated(rapidjson::Value& value, const generic_heirarchy& root, rapidjson::Document& doc)
	//{
	//	for (const generic_heirarchy& child : root.m_children)
	//	{
	//		if (child.has_name())
	//		{
	//			if (child.has_single_value())
	//			{
	//				rapidjson::GenericValue<rapidjson::UTF8<> > name(child.get_name(), doc.GetAllocator());
	//				std::string child_value_as_string = child.get_single_child_value();
	//				rapidjson::GenericValue<rapidjson::UTF8<> > child_value(child_value_as_string.c_str(), doc.GetAllocator());
	//				value.AddMember(name, child_value, doc.GetAllocator());
	//			}
	//			else if (child.all_children_unnamed_parents())
	//			{
	//				rapidjson::GenericValue<rapidjson::UTF8<> > name(child.get_name(), doc.GetAllocator());
	//				rapidjson::Value obj;
	//				obj.SetArray();
	//				for (const generic_heirarchy& grandchild : child.m_children)
	//				{
	//					add_children(obj, grandchild, doc);
	//				}
	//				value.AddMember(name, obj, doc.GetAllocator());
	//			}
	//			else
	//			{
	//				//rapidjson::Value::GenericValue name = child.get_name();
	//				rapidjson::Value child_value = create_json_value(child, doc);
	//				if (value.IsArray())
	//				{
	//					value.PushBack(child_value, doc.GetAllocator());
	//				}
	//				else
	//				{
	//					rapidjson::GenericValue<rapidjson::UTF8<> > name(child.get_name(), doc.GetAllocator());
	//					value.AddMember(name, child_value, doc.GetAllocator());
	//				}
	//			}
	//		}
	//		else
	//		{
	//			if (value.IsArray())
	//			{
	//				rapidjson::Value obj;
	//				obj.SetArray();
	//				add_children(obj, child, doc);
	//				value.PushBack(obj, doc.GetAllocator());
	//			}
	//			else
	//			{
	//				rapidjson::GenericValue<rapidjson::UTF8<> > name("MISSING NAME", doc.GetAllocator());
	//				rapidjson::Value child_value = create_json_value(child, doc);
	//				value.AddMember(name, child_value, doc.GetAllocator());
	//			}
	//			//rapidjson::GenericValue<rapidjson::UTF8<> > name("???", doc.GetAllocator());
	//			//rapidjson::Value child_value = create_json_value(child, doc);
	//			//value.AddMember(name, child_value, doc.GetAllocator());
	//		}
	//	}
	//}

	//rapidjson::Value create_json_value_deprecated(const generic_heirarchy& node, rapidjson::Document& doc)
	//{
	//	rapidjson::Value value;
	//	if (node.m_children.empty())
	//	{
	//		//value.SetString(node.get_name(), doc.GetAllocator());
	//	}
	//	else
	//	{
	//		value.SetObject();
	//		add_children(value, node, doc);
	//	}

	//	return value;
	//}
};

int main()
{
	const char* json_text = "{\"name\":\"John Smith\",\"programmer\":true,\"pi\":3.14159,\"age\":30,\"cars\":[{\"name\":\"Ford\",\"models\":[\"Fiesta\",\"Focus\",\"Mustang\"]},{\"name\":\"BMW\",\"models\":[\"320\",\"X3\",\"X5\"]},{\"name\":\"Fiat\",\"models\":[\"500\",\"Panda\"]}]}";
	handle_json json_handler;
	generic_heirarchy* h = json_handler.produce_heirarchy(json_text);
	
	//handle_text text_handler;
	//std::cout << text_handler.consume_heirarchy(*h) << std::endl;

	//std::ostringstream ss;
	//h->dump(ss);
	//std::cout << ss.str() << std::endl;

	std::cout << json_handler.consume_heirarchy(*h) << std::endl;

	delete h;
}