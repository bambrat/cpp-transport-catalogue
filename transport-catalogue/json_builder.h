#pragma once

#include "json.h"
#include <stack>
#include <string>
#include <memory>

namespace json {
	class Builder {
	private:
		Node root_;
		std::vector<std::unique_ptr<Node>> nodes_;
		void add_node(const Node& node);

		class BaseContext;
		class KeyContext;
		class DictionaryContext;
		class ArrayContext;

	public:

		KeyContext key(const std::string& key);
		Builder& value(const Node::Value& value);

		DictionaryContext startDict();
		Builder& endDict();

		ArrayContext startArray();
		Builder& endArray();

		Node build();
	};

	class Builder::BaseContext {
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

	class Builder::KeyContext : public BaseContext {
	public:
		KeyContext(Builder& builder);

		KeyContext key(const std::string& key) = delete;

		BaseContext endDict() = delete;
		BaseContext endArray() = delete;

		DictionaryContext value(const Node::Value& value);
	};

	class Builder::DictionaryContext : public BaseContext {
	public:
		DictionaryContext(Builder& builder);

		DictionaryContext startDict() = delete;

		ArrayContext startArray() = delete;
		Builder& endArray() = delete;

		Builder& value(const Node::Value& value) = delete;
	};

	class Builder::ArrayContext : public BaseContext {
	public:
		ArrayContext(Builder& builder);

		KeyContext key(const std::string& key) = delete;

		Builder& endDict() = delete;

		ArrayContext value(const Node::Value& value);
	};

}//end namespace transport_catalogue
