#pragma once
#define _USE_MATH_DEFINES
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <set>
#include <deque>
#include <vector>
#include <string_view>
#include <cmath>
#include <optional>
#include <variant>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace svg {
	enum class StrokeLineCap {
		BUTT,
		ROUND,
		SQUARE,
	};

	enum class StrokeLineJoin {
		ARCS,
		BEVEL,
		MITER,
		MITER_CLIP,
		ROUND,
	};

	struct Rgb {
		Rgb() = default;
		Rgb(uint8_t red, uint8_t green, uint8_t blue)
			:red(red), green(green), blue(blue) {
		}
		uint8_t red = 0;
		uint8_t green = 0;
		uint8_t blue = 0;
	};

	inline std::ostream& operator<<(std::ostream& out, Rgb color) {
		using namespace std::literals;
		out << "rgb("sv << +color.red << ","sv << +color.green << ","sv << +color.blue << ")"sv;
		return out;
	}

	inline void printColor(std::ostream& out, Rgb& rgb);

	struct Rgba {
		Rgba() = default;
		Rgba(uint8_t red, uint8_t green, uint8_t blue, double opacity) :red(red), green(green), blue(blue), opacity(opacity) {}
		uint8_t red = 0;
		uint8_t green = 0;
		uint8_t blue = 0;
		double opacity = 1.0;
	};

	inline std::ostream& operator<<(std::ostream& out, Rgba color) {
		using namespace std::literals;
		out << "rgba("sv << +color.red << ","sv << +color.green << ","sv << +color.blue << ","sv << +color.opacity << ")"sv;
		return out;
	}

	inline void printColor(std::ostream& out, Rgba& rgba);

	using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;

	inline const Color NoneColor{ "none" };

	inline std::ostream& operator<<(std::ostream& out, StrokeLineCap stroke_line_cap);

	inline std::ostream& operator<<(std::ostream& out, StrokeLineJoin stroke_line_join);

	inline std::ostream& operator<<(std::ostream& out, Color color);

	template <typename Owner>
	class PathProps {
	public:
		Owner& setFillColor(Color color) {
			fill_color_ = std::move(color);
			return asOwner();
		}

		Owner& setStrokeColor(Color color) {
			stroke_color_ = std::move(color);
			return asOwner();
		}

		Owner& setStrokeWidth(double width) {
			stroke_width_ = width;
			return asOwner();
		}

		Owner& setStrokeLineCap(StrokeLineCap line_cap) {
			line_cap_ = line_cap;
			return asOwner();
		}

		Owner& setStrokeLineJoin(StrokeLineJoin line_join) {
			stroke_line_join_ = line_join;
			return asOwner();
		}

	protected:
		~PathProps() = default;

		void renderAttrs(std::ostream& out) const {
			using namespace std::literals;
			if (fill_color_) {
				out << "fill=\""sv << *fill_color_ << "\""sv;
			}
			if (stroke_color_) {
				out << " stroke=\""sv << *stroke_color_ << "\""sv;
			}
			if (stroke_width_) {
				out << " stroke-width=\""sv << *stroke_width_ << "\""sv;
			}
			if (line_cap_) {
				out << " stroke-linecap=\""sv << *line_cap_ << "\""sv;
			}
			if (stroke_line_join_) {
				out << " stroke-linejoin=\""sv << *stroke_line_join_ << "\""sv;
			}
		}

	private:
		Owner& asOwner() {
			return static_cast<Owner&>(*this);
		}

		std::optional<double> stroke_width_;
		std::optional<StrokeLineCap> line_cap_;
		std::optional<StrokeLineJoin> stroke_line_join_;
		std::optional<Color> fill_color_;
		std::optional<Color> stroke_color_;
	};


	struct Point {
		Point() = default;
		Point(double x, double y)
			: x(x)
			, y(y) {
		}
		double x = 0;
		double y = 0;
	};

	struct RenderContext {
		RenderContext(std::ostream& out) : out(out) {}

		RenderContext(std::ostream& out, int indent_step, int indent = 0)
			: out(out)
			, indent_step(indent_step)
			, indent(indent) {
		}

		RenderContext indented() const {
			return { out, indent_step, indent + indent_step };
		}

		void renderIndent() const {
			for (int i = 0; i < indent; ++i) {
				out.put(' ');
			};
		}

		std::ostream& out;
		int indent_step = 0;
		int indent = 0;
	};

	class Object {
	public:
		virtual void render(const RenderContext& context) const;
		virtual ~Object() = default;
	private:
		virtual void renderObject(const RenderContext& context) const = 0;
	};

	class ObjectContainer {
	public:
		template <typename Obj>
		void add(Obj obj) {
			addPtr(std::make_unique<Obj>(std::move(obj)));
		}
		virtual void addPtr(std::unique_ptr<Object>&&) = 0;
		virtual ~ObjectContainer() = default;
	protected:
		std::vector<std::unique_ptr<Object>> objects_;
	};

	class Drawable {
	public:
		virtual void draw(ObjectContainer& container) const = 0;
		virtual ~Drawable() = default;
	};

	class Circle final : public Object, public PathProps<Circle> {
	public:
		Circle& setCenter(Point center = { 0.0, 0.0 });
		Circle& setRadius(double radius = 1.0);

	private:
		void renderObject(const RenderContext& context) const override;

		Point center_;
		double radius_ = 1.0;
	};

	class Polyline final : public Object, public PathProps<Polyline> {
	public:
		Polyline& addPoint(Point point);
	private:
		std::vector<Point> points_;
		void renderObject(const RenderContext& context) const override;
	};

	class Text final : public Object, public PathProps<Text> {
	public:
		Text& setPosition(Point pos = { 0.0, 0.0 });
		Text& setOffset(Point offset = { 0.0, 0.0 });
		Text& setFontSize(uint32_t size = 1);
		Text& setFontFamily(std::string font_family);
		Text& setFontWeight(std::string font_weight);
		Text& setData(std::string data = "");
	private:
		Point position_;
		Point offset_;
		std::string font_family_;
		std::string font_weight_;
		uint32_t font_size_ = 1;
		std::string data_;

		static std::string deleteSpaces(const std::string& str);
		static std::string uniqSymbols(const std::string& str);

		void renderObject(const RenderContext& ctx) const override;
	};

	class Document : public ObjectContainer {
	public:
		void addPtr(std::unique_ptr<Object>&& obj) override {
			objects_.push_back(std::move(obj));
		}
		void render(std::ostream& out) const;
	private:
		std::vector<std::unique_ptr<Object>> objects_;
	};
}  // end namespace svg