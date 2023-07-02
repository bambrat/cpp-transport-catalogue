#pragma once
#include "domain.h"
#include "geo.h"
#include "svg.h"
#include "json.h"

#include <sstream>


namespace map_renderer {
    inline const double EPSILON = 1e-6;

    class SphereProjector {
    public:
        template <typename InputIt>
        SphereProjector(InputIt points_begin, InputIt points_end, double max_width, double max_height, double padding);
        svg::Point operator()(geo::Coordinates coords) const;

    private:
        double padding_;
        double min_lon_ = 0;
        double max_lat_ = 0;
        double zoom_coeff_ = 0;

        bool is_zero(double value);
    };

    class MapRenderer {
    public:
        explicit MapRenderer(const transport::domain::RenderSettings& render_settings);
        const std::string getMapJson(const transport::domain::BusMap& buses, const transport::domain::StopMap& stops) const;

    private:
        SphereProjector getSphereProjector(const std::vector<geo::Coordinates>& points) const;
        
        void addStopsInfo(svg::Document& doc, SphereProjector& sphere_projector, const transport::domain::StopMap& stops) const;
        void addLine(std::vector<std::pair<const transport::domain::Bus*, int>>& buses_palette, svg::Document& doc, SphereProjector& sphere_projector) const;
        void addBusesName(std::vector<std::pair<const transport::domain::Bus*, int>>& buses_palette, svg::Document& doc, SphereProjector& sphere_projector) const;
        
        void setLineProperties(svg::Polyline& polyline, int line_number) const;
        void setRouteTextCommon(svg::Text& text, const std::string& name, svg::Point position) const;
        void setStopsTextCommon(svg::Text& text, const std::string& name, svg::Point position) const;
        void setStopsTextAdditional(svg::Text& text, const std::string& name, svg::Point position) const;
        void setStopsTextColor(svg::Text& text, const std::string& name, svg::Point position) const;
        void setRouteTextAdditional(svg::Text& text, const std::string& name, svg::Point position) const;
        void setRouteTextColor(svg::Text& text, const std::string& name, int palette, svg::Point position) const;
        void setStopsCircles(svg::Circle& circle, svg::Point position) const;

        svg::Color getСolor(int line_number) const;
        int getPaletteSize() const;

        const transport::domain::RenderSettings& render_settings_;
    };

    template <typename InputIt>
    SphereProjector::SphereProjector(InputIt points_begin, InputIt points_end, double max_width, double max_height, double padding) : padding_(padding) {
        if (points_begin == points_end) {
            return;
        }

        const auto [left_it, right_it] = std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs) {
                    return lhs.lng < rhs.lng;
                });

        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        const auto [bottom_it, top_it] = std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs) {
                    return lhs.lat < rhs.lat;
                });

        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        std::optional<double> width_zoom;
        if (!is_zero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        std::optional<double> height_zoom;
        if (!is_zero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        }
        else if (width_zoom) {
            zoom_coeff_ = *width_zoom;

        }
        else if (height_zoom) {
            zoom_coeff_ = *height_zoom;

        }
    }

}//end namespace map_renderer
