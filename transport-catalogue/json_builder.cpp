#include "json_builder.h"

namespace json {

	BaseContext::BaseContext(Builder& builder) : builder_(builder) {}

	KeyContext BaseContext::key(const std::string& key) { return builder_.key(key); }
	Builder& BaseContext::value(const Node::Value& value) { return builder_.value(value); }

	DictionaryContext BaseContext::startDict() { return DictionaryContext(builder_.startDict()); }
	Builder& BaseContext::endDict() { return builder_.endDict(); }

	ArrayContext BaseContext::startArray() { return ArrayContext(builder_.startArray()); }
	Builder& BaseContext::endArray() { return builder_.endArray(); }

	KeyContext::KeyContext(Builder& builder) : BaseContext(builder) {}

	DictionaryContext KeyContext::value(const Node::Value& value) { return BaseContext::value(value); }

	DictionaryContext::DictionaryContext(Builder& builder) : BaseContext(builder) {}

	ArrayContext::ArrayContext(Builder& builder) : BaseContext(builder) {}

	ArrayContext ArrayContext::value(const Node::Value& value) { return BaseContext::value(value); }

	Node Builder::make_node(const Node::Value& value_) {
		Node node;

		if (std::holds_alternative<bool>(value_)) node = Node(std::get<bool>(value_));
		else if (std::holds_alternative<int>(value_)) node = Node(std::get<int>(value_));
		else if (std::holds_alternative<double>(value_)) node = Node(std::get<double>(value_));
		else if (std::holds_alternative<std::string>(value_)) node = Node((std::get<std::string>(value_)));
		else if (std::holds_alternative<Array>(value_)) node = Node((std::get<Array>(value_)));
		else if (std::holds_alternative<Dict>(value_)) node = Node((std::get<Dict>(value_)));
		else node = Node();

		return node;
	}

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

	KeyContext Builder::key(const std::string& key_) {
		if (nodes_.empty()) throw std::logic_error("unable to create key");

		auto key_ptr = std::make_unique<Node>(key_);
		if (nodes_.back()->isDict()) nodes_.emplace_back(std::move(key_ptr));

		return KeyContext(*this);
	}

	Builder& Builder::value(const Node::Value& value_) {
		add_node(make_node(value_));
		return *this;
	}

	DictionaryContext Builder::startDict() {
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

	ArrayContext Builder::startArray() {
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

}//end namespace transport_catalogue
