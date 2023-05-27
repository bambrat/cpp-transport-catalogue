#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {
    using namespace std::literals;

    class Node;

    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node {
    public:

        using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

        Node() = default;
        Node(bool value);
        Node(Array array);
        Node(Dict map);
        Node(int value);
        Node(std::string value);
        Node(std::nullptr_t);
        Node(double value);

        const Array& asArray() const;
        bool asBool() const;
        double asDouble() const;
        int asInt() const;
        const Dict& asMap() const;
        const std::string& asString() const;

        bool isArray() const;
        bool isBool() const;
        bool isDouble() const;
        bool isInt() const;
        bool isMap() const;
        bool isNull() const;
        bool isPureDouble() const;
        bool isString() const;

        const Value& getValue() const { return value_; };

    private:
        Value value_;
    };

    inline bool operator==(const Node& lhs, const Node& rhs) {
        return lhs.getValue() == rhs.getValue();
    }
    inline bool operator!=(const Node& lhs, const Node& rhs) {
        return !(lhs == rhs);
    }

    class Document {
    public:
        Document() = default;
        explicit Document(Node root);
        const Node& getRoot() const;

    private:
        Node root_;
    };

    Document load(std::istream& input);

    inline bool operator==(const Document& lhs, const Document& rhs) {
        return lhs.getRoot() == rhs.getRoot();
    }
    inline bool operator!=(const Document& lhs, const Document& rhs) {
        return !(lhs == rhs);
    }

    void print(const Document& doc, std::ostream& output);

}//end namespace json
