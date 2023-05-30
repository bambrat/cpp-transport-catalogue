#include "json.h"

#include <iterator>

namespace json {

    namespace {
        using namespace std::literals;

        Node loadNode(std::istream& input);
        Node loadString(std::istream& input);

        std::string loadLiteral(std::istream& input) {
            std::string s;
            while (std::isalpha(input.peek())) {
                s.push_back(static_cast<char>(input.get()));
            }
            return s;
        }

        Node loadArray(std::istream& input) {
            std::vector<Node> result;

            for (char c; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(loadNode(input));
            }
            if (!input) {
                throw ParsingError("Array parsing error"s);
            }
            return Node(std::move(result));
        }

        Node loadDict(std::istream& input) {
            Dict dict;

            for (char c; input >> c && c != '}';) {
                if (c == '"') {
                    std::string key = loadString(input).asString();
                    if (input >> c && c == ':') {
                        if (dict.find(key) != dict.end()) {
                            throw ParsingError("Duplicate key '"s + key + "' have been found");
                        }
                        dict.emplace(std::move(key), loadNode(input));
                    }
                    else {
                        throw ParsingError(": is expected but '"s + c + "' has been found"s);
                    }
                }
                else if (c != ',') {
                    throw ParsingError(R"(',' is expected but ')"s + c + "' has been found"s);
                }
            }
            if (!input) {
                throw ParsingError("Dictionary parsing error"s);
            }
            return Node(std::move(dict));
        }

        Node loadString(std::istream& input) {
            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    ++it;
                    if (it == end) {
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    switch (escaped_char) {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r') {
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    s.push_back(ch);
                }
                ++it;
            }

            return Node(std::move(s));
        }

        Node loadBool(std::istream& input) {
            const auto s = loadLiteral(input);
            if (s == "true"sv) {
                return Node{ true };
            }
            else if (s == "false"sv) {
                return Node{ false };
            }
            else {
                throw ParsingError("Failed to parse '"s + s + "' as bool"s);
            }
        }

        Node loadNull(std::istream& input) {
            if (auto literal = loadLiteral(input); literal == "null"sv) {
                return Node{ nullptr };
            }
            else {
                throw ParsingError("Failed to parse '"s + literal + "' as null"s);
            }
        }

        Node loadNumber(std::istream& input) {
            std::string parsed_num;

            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
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
                        return std::stoi(parsed_num);
                    }
                    catch (...) {
                    }
                }
                return std::stod(parsed_num);
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        Node loadNode(std::istream& input) {
            char c;
            if (!(input >> c)) {
                throw ParsingError("Unexpected EOF"s);
            }
            switch (c) {
            case '[':
                return loadArray(input);
            case '{':
                return loadDict(input);
            case '"':
                return loadString(input);
            case 't':
                [[fallthrough]];
            case 'f':
                input.putback(c);
                return loadBool(input);
            case 'n':
                input.putback(c);
                return loadNull(input);
            default:
                input.putback(c);
                return loadNumber(input);
            }
        }

        struct PrintContext {
            std::ostream& out;
            int indent_step = 4;
            int indent = 0;

            void printIndent() const {
                for (int i = 0; i < indent; ++i) {
                    out.put(' ');
                }
            }

            PrintContext Indented() const {
                return { out, indent_step, indent_step + indent };
            }
        };

        void printNode(const Node& value, const PrintContext& ctx);

        template <typename Value>
        void printValue(const Value& value, const PrintContext& ctx) {
            ctx.out << value;
        }

        void printString(const std::string& value, std::ostream& out) {
            out.put('"');
            for (const char c : value) {
                switch (c) {
                case '\r':
                    out << "\\r"sv;
                    break;
                case '\n':
                    out << "\\n"sv;
                    break;
                case '"':
                    [[fallthrough]];
                case '\\':
                    out.put('\\');
                    [[fallthrough]];
                default:
                    out.put(c);
                    break;
                }
            }
            out.put('"');
        }

        template <>
        void printValue<std::string>(const std::string& value, const PrintContext& ctx) {
            printString(value, ctx.out);
        }

        template <>
        void printValue<std::nullptr_t>(const std::nullptr_t&, const PrintContext& ctx) {
            ctx.out << "null"sv;
        }

        template <>
        void printValue<bool>(const bool& value, const PrintContext& ctx) {
            ctx.out << (value ? "true"sv : "false"sv);
        }

        template <>
        void printValue<Array>(const Array& nodes, const PrintContext& ctx) {
            std::ostream& out = ctx.out;
            out << "[\n"sv;
            bool first = true;
            auto inner_ctx = ctx.Indented();
            for (const Node& node : nodes) {
                if (first) {
                    first = false;
                }
                else {
                    out << ",\n"sv;
                }
                inner_ctx.printIndent();
                printNode(node, inner_ctx);
            }
            out.put('\n');
            ctx.printIndent();
            out.put(']');
        }

        template <>
        void printValue<Dict>(const Dict& nodes, const PrintContext& ctx) {
            std::ostream& out = ctx.out;
            out << "{\n"sv;
            bool first = true;
            auto inner_ctx = ctx.Indented();
            for (const auto& [key, node] : nodes) {
                if (first) {
                    first = false;
                }
                else {
                    out << ",\n"sv;
                }
                inner_ctx.printIndent();
                printString(key, ctx.out);
                out << ": "sv;
                printNode(node, inner_ctx);
            }
            out.put('\n');
            ctx.printIndent();
            out.put('}');
        }

        void printNode(const Node& node, const PrintContext& ctx) {
            std::visit(
                [&](const auto& value) {
                    printValue(value, ctx);
                },
                node.getValue());
        }

    }  // namespace

    Document load(std::istream& input) {
        return Document{ loadNode(input) };
    }

    void print(const Document& doc, std::ostream& output) {
        printNode(doc.getRoot(), PrintContext{ output });
    }

}  // namespace json