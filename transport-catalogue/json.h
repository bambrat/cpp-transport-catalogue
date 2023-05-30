#pragma once

#include <iostream>
#include <map>
#include <string>
<<<<<<< HEAD
#include <variant>
#include <vector>

namespace json {

    class Node;
=======
#include <vector>
#include <variant>

namespace json {
    class Node;

>>>>>>> 10ab7554ddb88db1f9df3aa1be3851c045b11d50
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

<<<<<<< HEAD
    class Node final
        : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
    public:
        using variant::variant;
        using Value = variant;

        bool isInt() const {
            return std::holds_alternative<int>(*this);
        }
        int asInt() const {
            using namespace std::literals;
            if (!isInt()) {
                throw std::logic_error("Not an int"s);
            }
            return std::get<int>(*this);
        }

        bool isPureDouble() const {
            return std::holds_alternative<double>(*this);
        }
        bool isDouble() const {
            return isInt() || isPureDouble();
        }
        double asDouble() const {
            using namespace std::literals;
            if (!isDouble()) {
                throw std::logic_error("Not a double"s);
            }
            return isPureDouble() ? std::get<double>(*this) : asInt();
        }

        bool isBool() const {
            return std::holds_alternative<bool>(*this);
        }
        bool asBool() const {
            using namespace std::literals;
            if (!isBool()) {
                throw std::logic_error("Not a bool"s);
            }

            return std::get<bool>(*this);
        }

        bool isNull() const {
            return std::holds_alternative<std::nullptr_t>(*this);
        }

        bool isArray() const {
            return std::holds_alternative<Array>(*this);
        }
        const Array& asArray() const {
            using namespace std::literals;
            if (!isArray()) {
                throw std::logic_error("Not an array"s);
            }

            return std::get<Array>(*this);
        }

        bool isString() const {
            return std::holds_alternative<std::string>(*this);
        }
        const std::string& asString() const {
            using namespace std::literals;
            if (!isString()) {
                throw std::logic_error("Not a string"s);
            }

            return std::get<std::string>(*this);
        }

        bool isDict() const {
            return std::holds_alternative<Dict>(*this);
        }
        const Dict& asDict() const {
            using namespace std::literals;
            if (!isDict()) {
                throw std::logic_error("Not a dict"s);
            }

            return std::get<Dict>(*this);
        }

        bool operator==(const Node& rhs) const {
            return getValue() == rhs.getValue();
        }

        const Value& getValue() const {
            return *this;
        }
    };

=======
    class Node {
    public:

        using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

        Node() = default;

        template <typename Obj>
        Node(Obj obj);
        
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

    template <typename Obj>
    Node::Node(Obj obj) : value_(std::move(obj)) {}

    inline bool operator==(const Node& lhs, const Node& rhs) {
        return lhs.getValue() == rhs.getValue();
    }
>>>>>>> 10ab7554ddb88db1f9df3aa1be3851c045b11d50
    inline bool operator!=(const Node& lhs, const Node& rhs) {
        return !(lhs == rhs);
    }

    class Document {
    public:
<<<<<<< HEAD
        explicit Document(Node root)
            : root_(std::move(root)) {
        }

        const Node& getRoot() const {
            return root_;
        }
=======
        Document() = default;
        explicit Document(Node root);
        const Node& getRoot() const;
>>>>>>> 10ab7554ddb88db1f9df3aa1be3851c045b11d50

    private:
        Node root_;
    };

<<<<<<< HEAD
    inline bool operator==(const Document& lhs, const Document& rhs) {
        return lhs.getRoot() == rhs.getRoot();
    }

=======
    Document load(std::istream& input);

    inline bool operator==(const Document& lhs, const Document& rhs) {
        return lhs.getRoot() == rhs.getRoot();
    }
>>>>>>> 10ab7554ddb88db1f9df3aa1be3851c045b11d50
    inline bool operator!=(const Document& lhs, const Document& rhs) {
        return !(lhs == rhs);
    }

<<<<<<< HEAD
    Document load(std::istream& input);

    void print(const Document& doc, std::ostream& output);

}  // namespace json
=======
    void print(const Document& doc, std::ostream& output);

}//end namespace json
>>>>>>> 10ab7554ddb88db1f9df3aa1be3851c045b11d50
