#include "request_handler.h"

namespace request_handler {

    std::vector<geo::Coordinates> getStopsCoord(BusMap buses) {
        std::vector<geo::Coordinates> stops_coordinates;

        for (auto& [busname, bus] : buses) {
            for (auto& stop : bus->stops) {
                stops_coordinates.push_back(stop->coord);
            }
        }
        return stops_coordinates;
    }
    
    RequestHandler::RequestHandler(TransportCatalogue& db, const MapRenderer& renderer) : db_(db), map_renderer_(renderer) {}

    Dict RequestHandler::creatingMapNode(int id) const {
        Dict result;

        result.emplace("request_id", Node{ id });

        svg::Document document;

        auto buses = db_.getAllBuses();

        auto sphere_projector = map_renderer_.getSphereProjector(getStopsCoord(buses));
        std::vector<std::string_view> buses_name;
        std::vector<std::pair<const Bus*, int>> buses_palette;
        std::vector<geo::Coordinates> stops_geo_coords;

        int palette_size = 0;
        int palette_index = 0;

        palette_size = map_renderer_.getPaletteSize();
        if (palette_size == 0) {
            std::cout << "color palette is empty";
            return result;
        }
        
        if (buses.size() > 0) {
            std::vector<std::string_view> buses_names;
            for (auto& [busname, bus] : buses) buses_names.push_back(busname);

            std::sort(buses_names.begin(), buses_names.end());

            for (std::string_view bus_name : buses_names) {
                auto bus_info = db_.getBus(bus_name);

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
                map_renderer_.addLine(buses_palette, document, sphere_projector);
                map_renderer_.addBusesName(buses_palette, document, sphere_projector);
            }
        }

        map_renderer_.addStopsIcons(document, sphere_projector , db_.getAllStops());
        map_renderer_.addStopsName(document, sphere_projector, db_.getAllStops());
        std::stringstream map_svg;
        document.render(map_svg);
        result.emplace("map", Node{ map_svg.str()});
        return result;
    }
    
    void RequestHandler::printResponseData(json::Array stat_requests)
    {
        using namespace std::string_literals;

        std::vector<Node> result_request;
        for (auto& request : stat_requests) {
            const std::string& type = request.asMap().at("type"s).asString();
            auto id = request.asMap().at("id"s).asInt();
            std::string not_fount = "not found";

            Dict result;
            if (type == "Stop"s) {
                auto& name = request.asMap().at("name"s).asString();
                auto stop = db_.getStop(name);
                Array buses;

                result.emplace("request_id", Node{ id });

                if (stop == nullptr) {
                    result.emplace("error_message", Node{ not_fount });
                }
                else {
                    auto& unique_buses = stop->unique_buses;
                    if (unique_buses.size() != 0) {
                        for (auto& bus : unique_buses) {
                            buses.push_back(Node{ std::string(bus) });
                        }
                    }
                    result.emplace("buses", Node{ buses });
                }
            }
            else if (type == "Bus") {
                auto& name = request.asMap().at("name"s).asString();
                auto bus = db_.getBus(name);

                result.emplace("request_id", Node{ id });
                if (bus == nullptr) {
                    result.emplace("error_message", Node{ not_fount });
                }
                else {
                    result.emplace("curvature", Node{ bus->curvature });
                    result.emplace("route_length", Node{ bus->route_length });
                    result.emplace("stop_count", Node{ int(bus->stops.size()) });
                    result.emplace("unique_stop_count", Node{ int(bus->unique_stops.size()) });
                }
            }
            else if (type == "Map") {
                result = creatingMapNode(id);
            }
            result_request.push_back(result);
        }
        print(Document{ result_request }, std::cout);
    }

}//end namespace request_handler