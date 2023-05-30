#pragma once

#include "json.h"
#include <stack>
#include <string>
#include <memory>

namespace json {
	class KeyContext;
	class DictionaryContext;
	class ArrayContext;

	class Builder {
	public:
		Node make_node(const Node::Value& value_);
		void add_node(const Node& node);

		KeyContext key(const std::string& key_);
		Builder& value(const Node::Value& value);

		DictionaryContext startDict();
		Builder& endDict();

		ArrayContext startArray();
		Builder& endArray();

		Node build();

	private:
		Node root_;
		std::vector<std::unique_ptr<Node>> nodes_;
	};

	class BaseContext {
	public:
		BaseContext(Builder& builder);

		KeyContext key(const std::string& key);
		Builder& value(const Node::Value& value);

		DictionaryContext startDict();
		Builder& endDict();

		ArrayContext startArray();
		Builder& endArray();

	protected:
		Builder& builder_;
	};

	class KeyContext : public BaseContext {
	public:
		KeyContext(Builder& builder);

		KeyContext key(const std::string& key) = delete;

		BaseContext endDict() = delete;
		BaseContext endArray() = delete;

		DictionaryContext value(const Node::Value& value);
	};

	class DictionaryContext : public BaseContext {
	public:
		DictionaryContext(Builder& builder);

		DictionaryContext startDict() = delete;

		ArrayContext startArray() = delete;
		Builder& endArray() = delete;

		Builder& value(const Node::Value& value) = delete;
	};

	class ArrayContext : public BaseContext {
	public:
		ArrayContext(Builder& builder);

		KeyContext key(const std::string& key) = delete;

		Builder& endDict() = delete;

		ArrayContext value(const Node::Value& value);
	};

}//end namespace transport_catalogue
