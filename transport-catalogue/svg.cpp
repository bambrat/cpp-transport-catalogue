#include "svg.h"

namespace svg {
	using namespace std::literals;

	inline void printColor(std::ostream& out, Rgb& rgb) {
		out << "rgb("sv << static_cast<short>(rgb.red) << ","sv
			<< static_cast<short>(rgb.green) << ","sv
			<< static_cast<short>(rgb.blue) << ")"sv;
	}

	inline void printColor(std::ostream& out, Rgba& rgba) {
		out << "rgba("sv << static_cast<short>(rgba.red) << ","sv
			<< static_cast<short>(rgba.green) << ","sv
			<< static_cast<short>(rgba.blue) << ","sv
			<< (rgba.opacity) << ")"sv;
	}

	std::ostream& operator<<(std::ostream& out, StrokeLineCap stroke_line_cap)
	{
		if (stroke_line_cap == StrokeLineCap::BUTT) {
			out << "butt"sv;
		}
		else if (stroke_line_cap == StrokeLineCap::ROUND) {
			out << "round"sv;
		}
		else if (stroke_line_cap == StrokeLineCap::SQUARE) {
			out << "square"sv;
		}
		return out;
	}

	std::ostream& operator<<(std::ostream& out, StrokeLineJoin stroke_line_join)
	{
		if (stroke_line_join == StrokeLineJoin::ARCS) {
			out << "arcs"sv;
		}
		else if (stroke_line_join == StrokeLineJoin::BEVEL) {
			out << "bevel"sv;
		}
		else if (stroke_line_join == StrokeLineJoin::MITER) {
			out << "miter"sv;
		}
		else if (stroke_line_join == StrokeLineJoin::MITER_CLIP) {
			out << "miter-clip"sv;
		}
		else if (stroke_line_join == StrokeLineJoin::ROUND) {
			out << "round"sv;
		}
		return out;
	}

	std::ostream& operator<<(std::ostream& out, Color color)
	{
		if (std::holds_alternative<std::monostate>(color)) {
			out << "none"sv;
		}
		else if (std::holds_alternative<std::string>(color)) {
			out << std::get<std::string>(color);
		}
		else if (std::holds_alternative<Rgb>(color)) {
			out << std::get<Rgb>(color);
		}
		else if (std::holds_alternative<Rgba>(color)) {
			out << std::get<Rgba>(color);
		}
		return out;
	}

	inline void printColor(std::ostream& out, std::monostate) {
		out << "none"sv;
	}

	inline void printColor(std::ostream& out, std::string& color) {
		out << color;
	}

	std::ostream& operator<<(std::ostream& out, const Color& color) {
		std::visit([&out](auto value) { printColor(out, value); }, color);
		return out;
	}

	void Object::render(const RenderContext& context) const {
		context.renderIndent();
		renderObject(context);
		context.out << std::endl;
	}

	Circle& Circle::setCenter(Point center) {
		center_ = center;
		return *this;
	}

	Circle& Circle::setRadius(double radius) {
		radius_ = radius;
		return *this;
	}

	void Circle::renderObject(const RenderContext& context) const {
		std::ostream& out = context.out;

		out << "<circle cx=\""sv << center_.x
			<< "\" cy=\""sv << center_.y << "\" "sv;
		out << "r=\""sv << radius_ << "\" "sv;

		renderAttrs(context.out);
		out << "/>"sv;
	}

	Polyline& Polyline::addPoint(Point point) {
		points_.push_back(point);
		return *this;
	}

	void Polyline::renderObject(const RenderContext& context) const {
		std::ostream& out = context.out;
		out << "<polyline points=\""sv;

		for (size_t i = 0; i < points_.size(); ++i) {
			out << points_[i].x << ","sv << points_[i].y;

			if (i + 1 != points_.size()) {
				out << " "sv;
			}
		}
		out << "\" "sv;
		renderAttrs(context.out);
		out << "/>";
	}

	Text& Text::setPosition(Point pos) {
		position_ = pos;
		return *this;
	}

	Text& Text::setOffset(Point offset) {
		offset_ = offset;
		return *this;
	}

	Text& Text::setFontSize(uint32_t size) {
		font_size_ = size;
		return *this;
	}

	Text& Text::setFontFamily(std::string font_family) {
		font_family_ = std::move(font_family);
		return *this;
	}

	Text& Text::setFontWeight(std::string font_weight) {
		font_weight_ = std::move(font_weight);
		return *this;
	}

	Text& Text::setData(std::string data) {
		data_ = std::move(data);
		return *this;
	}

	std::string Text::deleteSpaces(const std::string& str) {
		if (str.empty()) {
			return {};
		}
		else {

			auto left = str.find_first_not_of(' ');
			auto right = str.find_last_not_of(' ');
			return str.substr(left, right - left + 1);
		}
	}

	std::string Text::uniqSymbols(const std::string& str) {

		std::string out;

		for (char ch : str) {

			if (ch == '"') {
				out += "&quot;"sv;
				continue;

			}
			else if (ch == '`' || ch == '\'') {
				out += "&apos;"sv;
				continue;

			}
			else if (ch == '<') {
				out += "&lt;"sv;
				continue;

			}
			else if (ch == '>') {
				out += "&gt;"sv;
				continue;

			}
			else if (ch == '&') {
				out += "&amp;"sv;
				continue;

			}
			else {

			}

			out += ch;
		}

		return out;
	}

	void Text::renderObject(const RenderContext& context) const {

		std::ostream& out = context.out;
		out << "<text "sv;
		renderAttrs(context.out);
		out << " x=\""sv
			<< position_.x << "\" y=\""sv
			<< position_.y << "\" "sv
			<< "dx=\""sv
			<< offset_.x << "\" dy=\""sv
			<< offset_.y << "\" "sv
			<< "font-size=\""sv
			<< font_size_ << "\" "sv;

		if (!font_family_.empty()) {
			out << "font-family=\""sv << font_family_ << "\""sv;
		}

		if (!font_weight_.empty()) {
			out << " font-weight=\""sv << font_weight_ << "\""sv;
		}

		out << ">"sv << deleteSpaces(uniqSymbols(data_)) << "</text>"sv;
	}

	void Document::render(std::ostream& out) const {
		int indent = 2;
		int indent_step = 2;

		RenderContext context(out, indent_step, indent);

		const std::string_view xml = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv;
		const std::string_view svg = "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv;

		out << xml << "\n"sv << svg << "\n"sv;

		for (const auto& object : objects_) {
			object->render(context);
		}
		out << "</svg>"sv;
	}
}//end namespace svg