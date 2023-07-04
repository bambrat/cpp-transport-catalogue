#include "json_builder.h"

namespace json {

	Builder::BaseContext::BaseContext(Builder& builder) : builder_(builder) {}

	Builder::KeyContext Builder::BaseContext::key(const std::string& key) { return builder_.key(key); }
	Builder& Builder::BaseContext::value(const Node::Value& value) { return builder_.value(value); }

	Builder::DictionaryContext Builder::BaseContext::startDict() { return Builder::DictionaryContext(builder_.startDict()); }
	Builder& Builder::BaseContext::endDict() { return builder_.endDict(); }

	Builder::ArrayContext Builder::BaseContext::startArray() { return Builder::ArrayContext(builder_.startArray()); }
	Builder& Builder::BaseContext::endArray() { return builder_.endArray(); }

	Builder::KeyContext::KeyContext(Builder& builder) : BaseContext(builder) {}

	Builder::DictionaryContext Builder::KeyContext::value(const Node::Value& value) { return BaseContext::value(value); }

	Builder::DictionaryContext::DictionaryContext(Builder& builder) : BaseContext(builder) {}

	Builder::ArrayContext::ArrayContext(Builder& builder) : BaseContext(builder) {}

	Builder::ArrayContext Builder::ArrayContext::value(const Node::Value& value) { return BaseContext::value(value); }

	void Builder::add_node(const Node& node) {
		if (nodes_.empty()) {
			if (!root_.isNull()) throw std::logic_error("root has been added");
			root_ = node;
			return;
		}
		
		if (!nodes_.back()->isArray() && !nodes_.back()->isString()) throw std::logic_error("unable to create node");

		if (nodes_.back()->isArray()) {
			auto arr = nodes_.back()->asArray();
			arr.emplace_back(node);

			nodes_.pop_back();
			auto arr_ptr = std::make_unique<Node>(arr);
			nodes_.emplace_back(std::move(arr_ptr));
			return;
		}

		std::string str = nodes_.back()->asString();
		nodes_.pop_back();

		if (nodes_.back()->isDict()) {
			auto dictionary = nodes_.back()->asDict();
			dictionary.emplace(std::move(str), node);

			nodes_.pop_back();
			auto dictionary_ptr = std::make_unique<Node>(dictionary);
			nodes_.emplace_back(std::move(dictionary_ptr));
		}
		return;
	}

	Builder::KeyContext Builder::key(const std::string& key) {
		if (nodes_.empty()) throw std::logic_error("unable to create key");

		auto key_ptr = std::make_unique<Node>(key);
		if (nodes_.back()->isDict()) nodes_.emplace_back(std::move(key_ptr));

		return KeyContext(*this);
	}

	Builder& Builder::value(const Node::Value& value) {
		add_node(Node(value));
		return *this;
	}

	Builder::DictionaryContext Builder::startDict() {
		nodes_.emplace_back(std::move(std::make_unique<Node>(Dict())));
		return DictionaryContext(*this);
	}

	Builder& Builder::endDict() {
		if (nodes_.empty()) throw std::logic_error("unable to close as without opening");
		auto node = *nodes_.back();
		if (!node.isDict()) throw std::logic_error("object isn't dictionary");

		nodes_.pop_back();
		add_node(node);

		return *this;
	}

	Builder::ArrayContext Builder::startArray() {
		nodes_.emplace_back(std::move(std::make_unique<Node>(Array())));
		return ArrayContext(*this);
	}

	Builder& Builder::endArray() {
		if (nodes_.empty()) throw std::logic_error("unable to close without opening");

		auto node = *nodes_.back();

		if (!node.isArray()) throw std::logic_error("object isn't array");

		nodes_.pop_back();
		add_node(node);

		return *this;
	}

	Node Builder::build() {
		if (root_.isNull()) throw std::logic_error("empty json");
		if (!nodes_.empty()) throw std::logic_error("invalid json");
		return root_;
	}

}//end namespace json
