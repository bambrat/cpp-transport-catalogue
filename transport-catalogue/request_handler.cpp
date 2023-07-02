#include "request_handler.h"

namespace request_handler {
    using namespace transport::catalogue;
    using namespace transport::router;
    using namespace transport::domain;
    using namespace map_renderer;

    RequestHandler::RequestHandler(TransportCatalogue& db, const MapRenderer& map_renderer, RouterByGraph& router_by_graph)
        : db_(db), map_renderer_(map_renderer), router_by_graph_(router_by_graph) {}

    json::Document RequestHandler::responseData(const json::Array& stat_requests)
    {
        using namespace std::string_literals;
        using namespace json;

        std::vector<Node> result_request;

        for (auto& request : stat_requests) {
            auto& rasd = request.asDict();
            auto& type = rasd.at("type"s).asString();
            auto id = rasd.at("id"s).asInt();
            auto not_fount = "not found"s;

            Builder result;
            result.startDict();

            if (type == "Stop"s) {
                auto stop = db_.getStop(rasd.at("name"s).asString());

                result.key("request_id").value(id);
                if (stop == nullptr) {
                    result.key("error_message").value(not_fount);
                }
                else {
                    auto& unique_buses = stop->unique_buses;
                    result.key("buses").startArray();
                    if (unique_buses.size() != 0) {
                        for (auto& bus : unique_buses) {
                            result.value(std::string(bus));
                        }
                    }
                    result.endArray();
                }
            }
            else if (type == "Bus") {
                auto bus = db_.getBus(rasd.at("name"s).asString());

                result.key("request_id").value(id);

                if (bus == nullptr) {
                    result.key("error_message").value(not_fount);
                }
                else {
                    result.key("curvature").value(bus->curvature).
                           key("route_length").value(bus->route_length).
                           key("stop_count").value(int(bus->stops.size())).
                           key("unique_stop_count").value(int(bus->unique_stops.size()));
                }
            }
            else if (type == "Map") {
                result.key("request_id").value(id);

                auto mp = map_renderer_.getMapJson(db_.getAllBuses(), db_.getAllStops());
                result.key("map").value(mp);

            }
            else if (type == "Route") {

                auto from = db_.getStop(request.asDict().at("from").asString());
                auto to = db_.getStop(request.asDict().at("to").asString());

                const auto& route_info = router_by_graph_.getRouteInfomation(router_by_graph_.getRouterByStop(from)->bus_wait_start,
                                                                             router_by_graph_.getRouterByStop(to)->bus_wait_start);
                
                result.key("request_id").value(id);

                if (!route_info) {
                    result.key("error_message"s).value(not_fount);
                }
                else {
                    Array items;
                    for (auto& item : route_info->edges) {
                        items.emplace_back(std::visit(EdgeInfo{}, item));
                    }
                    
                    result.key("total_time").value(route_info->total_time).key("items").value(items);
                }

            }
            result_request.push_back(result.endDict().build());
        }
        return Document{ result_request };
    }

    void RequestHandler::printResponse(const json::Array& stat_requests)
    {
        json::print( responseData (stat_requests) , std::cout);
    }

}//end namespace request_handler