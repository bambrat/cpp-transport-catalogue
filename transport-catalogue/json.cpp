#include "json.h"

namespace json {
    using namespace std::literals;
    namespace {

        Node loadNode(std::istream& input);

        std::string loadLiteral(std::istream& input) {
            std::string str;

            while (std::isalpha(input.peek())) {
                str.push_back(static_cast<char>(input.get()));
            }
            return str;
        }

        Node loadArray(std::istream& input) {
            std::vector<Node> array;

            for (char ch; input >> ch && ch != ']';) {
                if (ch != ',') {
                    input.putback(ch);
                }

                array.push_back(loadNode(input));
            }

            if (!input) {
                throw ParsingError("unable to parse array"s);
            }

            return Node(array);
        }

        Node loadNull(std::istream& input) {
            if (auto literal = loadLiteral(input); literal == "null"sv) {
                return Node(nullptr);
            }
            else {
                throw ParsingError("unable to parse '"s + literal + "' as null"s);
            }
        }

        Node loadBool(std::istream& input) {
            const auto str = loadLiteral(input);

            if (str == "true"sv) {
                return Node(true);
            }
            else if (str == "false"sv) {
                return Node(false);
            }
            else {
                throw ParsingError("unable to parse '"s + str + "' as bool"s);
            }
        }

        Node loadNumber(std::istream& input) {
            std::string number;

            auto read_char = [&number, &input] {
                number += static_cast<char>(input.get());

                if (!input) {
                    throw ParsingError("unable to read number"s);
                }
            };

            auto read_digits = [&input, read_char] {

                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                else {
                    while (std::isdigit(input.peek())) {
                        read_char();
                    }
                }
            };

            if (input.peek() == '-') {
                read_char();
            }

            if (input.peek() == '0') {
                read_char();
            }
            else {
                read_digits();
            }

            bool is_int = true;
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();

                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }

                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    try {
                        return Node(std::stoi(number));

                    }
                    catch (...) {}
                }
                return Node(std::stod(number));

            }
            catch (...) {
                throw ParsingError("unable to convert "s + number + " to number"s);
            }
        }

        Node loadString(std::istream& input) {
            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string str;

            while (true) {
                if (it == end) {
                    throw ParsingError("unable to parse string"s);
                }

                const char ch = *it;
                if (ch == '"') {
                    ++it;
                    break;

                }
                else if (ch == '\\') {
                    ++it;
                    if (it == end) {
                        throw ParsingError("unable to parse string"s);
                    }

                    const char esc_ch = *(it);
                    switch (esc_ch) {
                    case 'n':
                        str.push_back('\n');
                        break;
                    case 't':
                        str.push_back('\t');
                        break;
                    case 'r':
                        str.push_back('\r');
                        break;
                    case '"':
                        str.push_back('"');
                        break;
                    case '\\':
                        str.push_back('\\');
                        break;
                    default:
                        throw ParsingError("invalid esc \\"s + esc_ch);
                    }

                }
                else if (ch == '\n' || ch == '\r') {
                    throw ParsingError("invalid line end"s);
                }
                else {
                    str.push_back(ch);
                }

                ++it;
            }

            return Node(str);
        }

        Node loadDict(std::istream& input) {
            Dict dictionary;
            for (char ch; input >> ch && ch != '}';) {

                if (ch == '"') {
                    std::string key = loadString(input).asString();

                    if (input >> ch && ch == ':') {

                        if (dictionary.find(key) != dictionary.end()) {
                            throw ParsingError("duplicate key '"s + key + "'found");
                        }

                        dictionary.emplace(std::move(key), loadNode(input));

                    }
                    else {
                        throw ParsingError(": expected. but '"s + ch + "' found"s);
                    }

                }
                else if (ch != ',') {
                    throw ParsingError("',' expected. but '"s + ch + "' found"s);
                }
            }

            if (!input) {
                throw ParsingError("unable to parse dictionary"s);
            }
            else {
                return Node(dictionary);
            }

        }

        Node loadNode(std::istream& input) {
            char ch;
            if (!(input >> ch)) {
                throw ParsingError(""s);
            }
            else {
                switch (ch) {
                case '[':
                    return loadArray(input);
                case '{':
                    return loadDict(input);
                case '"':
                    return loadString(input);
                case 't': case 'f':
                    input.putback(ch);
                    return loadBool(input);
                case 'n':
                    input.putback(ch);
                    return loadNull(input);
                default:
                    input.putback(ch);
                    return loadNumber(input);
                }
            }
        }

    }//end namespace

    const Array& Node::asArray() const {
        if (!isArray()) {
            throw std::logic_error("value is not an array"s);
        }
        else {
            return std::get<Array>(value_);
        }
    }

    const Dict& Node::asDict() const {
        if (!isDict()) {
            throw std::logic_error("value is not a dictionary"s);
        }
        else {
            return std::get<Dict>(value_);
        }
    }

    const std::string& Node::asString() const {
        if (!isString()) {
            throw std::logic_error("value is not a string"s);
        }
        else {
            return std::get<std::string>(value_);
        }
    }

    int Node::asInt() const {
        if (!isInt()) {
            throw std::logic_error("value is not an int"s);
        }
        else {
            return std::get<int>(value_);
        }
    }

    double Node::asDouble() const {
        if (!isDouble()) {
            throw std::logic_error("value is not a double"s);
        }
        else if (isPureDouble()) {
            return std::get<double>(value_);
        }
        else {
            return asInt();
        }
    }

    bool Node::asBool() const {
        if (!isBool()) {
            throw std::logic_error("value is not a bool"s);
        }
        else {
            return std::get<bool>(value_);
        }
    }

    bool Node::isNull() const { return std::holds_alternative<std::nullptr_t>(value_); }
    bool Node::isInt() const { return std::holds_alternative<int>(value_); }
    bool Node::isDouble() const { return isPureDouble() || isInt(); }
    bool Node::isPureDouble() const { return std::holds_alternative<double>(value_); }
    bool Node::isBool() const { return std::holds_alternative<bool>(value_); }
    bool Node::isString() const { return std::holds_alternative<std::string>(value_); }
    bool Node::isArray() const { return std::holds_alternative<Array>(value_); }
    bool Node::isDict() const { return std::holds_alternative<Dict>(value_); }

    Document::Document(Node root) : root_(std::move(root)) {}
    const Node& Document::getRoot() const { return root_; }
    Document load(std::istream& input) {
        return Document{ loadNode(input) };
    }

    struct PrintContext {
        std::ostream& out;
        int indent_step = 4;
        int indent = 0;

        void PrintIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        [[nodiscard]] PrintContext Indented() const {
            return { out,
                    indent_step,
                    indent + indent_step };
        }
    };

    void printNode(const Node& node, const PrintContext& context);

    void printString(const std::string& value, std::ostream& out) {
        out.put('"');

        for (const char ch : value) {
            switch (ch) {
            case '\r':
                out << R"(\r)";
                break;
            case '\n':
                out << R"(\n)";
                break;
            case '"':
                out << R"(\")";
                break;
            case '\\':
                out << R"(\\)";
                break;
            default:
                out.put(ch);
                break;
            }
        }

        out.put('"');
    }

    template <typename Value>
    void printValue(const Value& value, const PrintContext& context) {
        context.out << value;
    }

    template <>
    void printValue<std::string>(const std::string& value, const PrintContext& context) {
        printString(value, context.out);
    }

    void printValue(const std::nullptr_t&, const PrintContext& context) {
        context.out << "null"s;
    }

    void printValue(bool value, const PrintContext& context) {
        context.out << std::boolalpha << value;
    }

    [[maybe_unused]] void printValue(Array nodes, const PrintContext& context) {
        std::ostream& out = context.out;
        out << "[\n"sv;
        bool first = true;
        auto inner_context = context.Indented();

        for (const Node& node : nodes) {
            if (first) {
                first = false;
            }
            else {
                out << ",\n"sv;
            }

            inner_context.PrintIndent();
            printNode(node, inner_context);
        }

        out.put('\n');
        context.PrintIndent();
        out.put(']');
    }

    [[maybe_unused]] void printValue(Dict nodes, const PrintContext& context) {
        std::ostream& out = context.out;
        out << "{\n"sv;
        bool first = true;
        auto inner_context = context.Indented();

        for (const auto& [key, node] : nodes) {
            if (first) {
                first = false;
            }
            else {
                out << ",\n"sv;
            }

            inner_context.PrintIndent();
            printString(key, context.out);
            out << ": "sv;
            printNode(node, inner_context);
        }

        out.put('\n');
        context.PrintIndent();
        out.put('}');
    }

    void printNode(const Node& node, const PrintContext& context) {
        std::visit([&context](const auto& value) {
            printValue(value, context);
            }, node.getValue());
    }

    void print(const Document& document, std::ostream& output) {
        printNode(document.getRoot(), PrintContext{ output });
    }
}//end namespace json