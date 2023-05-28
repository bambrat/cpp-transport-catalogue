#include "map_renderer.h"


namespace map_renderer {
    using namespace std::literals;

    bool SphereProjector::is_zero(double value) {
        return std::abs(value) < EPSILON;
    }

    MapRenderer::MapRenderer(const RenderSettings& render_settings) : render_settings_(render_settings) {}
    
    svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
        return { (coords.lng - min_lon_) * zoom_coeff_ + padding_,
                (max_lat_ - coords.lat) * zoom_coeff_ + padding_ };
    }

    SphereProjector MapRenderer::getSphereProjector(const std::vector<geo::Coordinates>& points) const {
        return SphereProjector(points.begin(), points.end(), render_settings_.width_,render_settings_.height_, render_settings_.padding_);
    }

    RenderSettings MapRenderer::getRenderSettings() const {
        return render_settings_;
    }

    int MapRenderer::getPaletteSize() const {
        return render_settings_.color_palette_.size();
    }

    svg::Color MapRenderer::get_color(int line_number) const {
        return render_settings_.color_palette_[line_number];
    }

    void MapRenderer::addLine(std::vector<std::pair<const transport::domain::Bus*, int>>& buses_palette, svg::Document& doc, SphereProjector& sphere_projector) const {
        for (auto& [bus, palette] : buses_palette) {
            std::vector<geo::Coordinates> stops_geo_coords;
            for (auto& stop : bus->stops) {
                stops_geo_coords.push_back(stop->coord);
            }

            svg::Polyline bus_line;

            for (auto& coord : stops_geo_coords) {
                bus_line.addPoint(sphere_projector(coord));
            }

            setLineProperties(bus_line, palette);
            doc.add(bus_line);
        }
    }

    void MapRenderer::setLineProperties(svg::Polyline& polyline, [[maybe_unused]] int line_number) const {
        
        polyline.setStrokeColor(get_color(line_number));
        polyline.setFillColor("none"s);
        polyline.setStrokeWidth(render_settings_.line_width_);
        polyline.setStrokeLineCap(svg::StrokeLineCap::ROUND);
        polyline.setStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    }

    void MapRenderer::addBusesName(std::vector<std::pair<const transport::domain::Bus*, int>>& buses_palette, svg::Document& doc, SphereProjector& sphere_projector) const {
        for (auto& [bus, palette] : buses_palette) {
            std::vector<geo::Coordinates> stops_geo_coords;

            for (auto& stop : bus->stops) {
                stops_geo_coords.push_back(stop->coord);
            }

            svg::Text route_name_roundtrip;
            svg::Text route_title_roundtrip;
            svg::Text route_name_notroundtrip;
            svg::Text route_title_notroundtrip;

            if (bus->is_roundtrip) {
                setRouteTextAdditional(route_name_roundtrip, std::string(bus->name), sphere_projector(stops_geo_coords[0]));
                doc.add(route_name_roundtrip);

                setRouteTextColor(route_title_roundtrip, std::string(bus->name), palette, sphere_projector(stops_geo_coords[0]));
                doc.add(route_title_roundtrip);
            }
            else {
                setRouteTextAdditional(route_name_roundtrip, std::string(bus->name), sphere_projector(stops_geo_coords[0]));
                doc.add(route_name_roundtrip);

                setRouteTextColor(route_title_roundtrip, std::string(bus->name), palette, sphere_projector(stops_geo_coords[0]));
                doc.add(route_title_roundtrip);

                if (stops_geo_coords[0] != stops_geo_coords[stops_geo_coords.size() / 2]) {
                    setRouteTextAdditional(route_name_notroundtrip, std::string(bus->name), sphere_projector(stops_geo_coords[stops_geo_coords.size() / 2]));
                    doc.add(route_name_notroundtrip);

                    setRouteTextColor(route_title_notroundtrip, std::string(bus->name), palette, sphere_projector(stops_geo_coords[stops_geo_coords.size() / 2]));
                    doc.add(route_title_notroundtrip);
                }
            }
        }
    }

    void MapRenderer::addStopsIcons(svg::Document& doc, SphereProjector& sphere_projector, const transport::domain::StopMap& stops) const {
        std::vector<std::string_view> stops_name;

        for (auto& [stop_name, stop] : stops) {
            if (stop->buses.size() > 0) {
                stops_name.push_back(stop_name);
            }
        }

        std::sort(stops_name.begin(), stops_name.end());

        svg::Circle icon;

        for (std::string_view stop_name : stops_name) {
            auto stop_info = stops.at(stop_name);

            if (stop_info) {
                setStopsCircles(icon, sphere_projector(stop_info->coord));
                doc.add(icon);
            }
        }
    }

    void MapRenderer::addStopsName(svg::Document& doc, SphereProjector& sphere_projector, const transport::domain::StopMap& stops) const {
        std::vector<std::string_view> stops_name;

        for (auto& [stop_name, stop] : stops) {

            if (stop->buses.size() > 0) {
                stops_name.push_back(stop_name);
            }
        }

        std::sort(stops_name.begin(), stops_name.end());

        svg::Text svg_stop_name;
        svg::Text svg_stop_name_title;

        for (std::string_view stop_name : stops_name) {
            auto stop_info = stops.at(stop_name); 

            if (stop_info) {
                setStopsTextAdditional(svg_stop_name, stop_info->name, sphere_projector(stop_info->coord));
                doc.add(svg_stop_name);

                setStopsTextColor(svg_stop_name_title, stop_info->name, sphere_projector(stop_info->coord));
                doc.add(svg_stop_name_title);
            }
        }
    }

    void MapRenderer::setRouteTextCommon(svg::Text& text, const std::string& name, svg::Point position) const {
        text.setPosition(position);
        text.setOffset({ render_settings_.bus_label_offset_.first,
                         render_settings_.bus_label_offset_.second });
        text.setFontSize(render_settings_.bus_label_font_size_);
        text.setFontFamily("Verdana"s);
        text.setFontWeight("bold"s);
        text.setData(name);
    }

    void MapRenderer::setRouteTextAdditional(svg::Text& text, const std::string& name, svg::Point position) const {
        setRouteTextCommon(text, name, position);

        text.setFillColor(render_settings_.underlayer_color_);
        text.setStrokeColor(render_settings_.underlayer_color_);
        text.setStrokeWidth(render_settings_.underlayer_width_);
        text.setStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        text.setStrokeLineCap(svg::StrokeLineCap::ROUND);
    }

    void MapRenderer::setRouteTextColor(svg::Text& text, const std::string& name, int palette, svg::Point position) const {
        setRouteTextCommon(text, name, position);
        text.setFillColor(get_color(palette));
    }

    void MapRenderer::setStopsCircles(svg::Circle& circle, svg::Point position) const {
        circle.setCenter(position);
        circle.setRadius(render_settings_.stop_radius_);
        circle.setFillColor("white"s);
    }

    void MapRenderer::setStopsTextCommon(svg::Text& text, const std::string& name, svg::Point position) const {
        text.setPosition(position);
        text.setOffset({ render_settings_.stop_label_offset_.first,
                         render_settings_.stop_label_offset_.second });
        text.setFontSize(render_settings_.stop_label_font_size_);
        text.setFontFamily("Verdana"s);
        text.setData(name);
    }

    void MapRenderer::setStopsTextAdditional(svg::Text& text, const std::string& name, svg::Point position) const {
        setStopsTextCommon(text, name, position);

        text.setFillColor(render_settings_.underlayer_color_);
        text.setStrokeColor(render_settings_.underlayer_color_);
        text.setStrokeWidth(render_settings_.underlayer_width_);
        text.setStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        text.setStrokeLineCap(svg::StrokeLineCap::ROUND);
    }

    void MapRenderer::setStopsTextColor(svg::Text& text, const std::string& name, svg::Point position) const {
        setStopsTextCommon(text, name, position);
        text.setFillColor("black"s);
    }

    std::vector<geo::Coordinates> getStopsCoord(transport::domain::BusMap buses) {
        std::vector<geo::Coordinates> stops_coordinates;

        for (auto& [busname, bus] : buses) {
            for (auto& stop : bus->stops) {
                stops_coordinates.push_back(stop->coord);
            }
        }
        return stops_coordinates;
    }

    json::Dict MapRenderer::creatingMapNode(int id, const transport::domain::BusMap& buses, const transport::domain::StopMap& stops ) const {
        json::Dict result;
        result.emplace("request_id", json::Node{ id });

        svg::Document document;
        auto sphere_projector = getSphereProjector(getStopsCoord(buses));
        std::vector<std::string_view> buses_name;
        std::vector<std::pair<const transport::domain::Bus*, int>> buses_palette;
        std::vector<geo::Coordinates> stops_geo_coords;

        int palette_size = 0;
        int palette_index = 0;

        palette_size = getPaletteSize();
        if (palette_size == 0) {
            std::cout << "color palette is empty";
            return result;
        }

        if (buses.size() > 0) {
            std::vector<std::string_view> buses_names;
            for (auto& [busname, bus] : buses) buses_names.push_back(busname);

            std::sort(buses_names.begin(), buses_names.end());

            for (std::string_view bus_name : buses_names) {
                auto bus_info = buses.at(bus_name);

                if (bus_info) {
                    if (bus_info->stops.size() > 0) {
                        buses_palette.push_back(std::make_pair(bus_info, palette_index));
                        palette_index++;

                        if (palette_index == palette_size) {
                            palette_index = 0;
                        }
                    }
                }
            }

            if (buses_palette.size() > 0) {
                addLine(buses_palette, document, sphere_projector);
                addBusesName(buses_palette, document, sphere_projector);
            }
        }

        addStopsIcons(document, sphere_projector, stops);
        addStopsName(document, sphere_projector, stops);
        std::stringstream map_svg;
        document.render(map_svg);
        result.emplace("map", json::Node{ map_svg.str() });
        return result;
    }
}//end namespace map_renderer