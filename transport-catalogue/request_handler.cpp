#include "request_handler.h"

namespace request_handler {

    RequestHandler::RequestHandler(transport::catalogue::TransportCatalogue& db, const map_renderer::MapRenderer& map_renderer_) : db_(db), map_renderer_(map_renderer_) {}

    void RequestHandler::printResponseData(const json::Array& stat_requests)
    {
        using namespace std::string_literals;
        using namespace json;

        std::vector<Node> result_request;
        
        for (auto& request : stat_requests) {
            auto& rasm = request.asDict();
            auto& type = rasm.at("type"s).asString();
            auto id = rasm.at("id"s).asInt();
            auto not_fount = "not found"s;

            Builder result; 
            result.startDict();
            
            if (type == "Stop"s) {
                auto& name = rasm.at("name"s).asString();
                auto stop = db_.getStop(name);
                
                result.key("request_id").value(id);
                if (stop == nullptr) {
                    result.key("error_message").value( not_fount );
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
                auto& name = rasm.at("name"s).asString();
                auto bus = db_.getBus(name);

                result.key("request_id").value(id);

                if (bus == nullptr) {
                    result.key("error_message").value(not_fount);
                }
                else {
                    result.key("curvature").value(bus->curvature);
                    result.key("route_length").value(bus->route_length);
                    result.key("stop_count").value(int(bus->stops.size()));
                    result.key("unique_stop_count").value(int(bus->unique_stops.size()));
                }
            }
            else if (type == "Map") {
                result.key("request_id").value(id);
                
                auto mp = map_renderer_.getMapJson(db_.getAllBuses(), db_.getAllStops());
                result.key("map").value(mp);
                
            }
            result_request.push_back(result.endDict().build());
        }
        json::print(Document{ result_request }, std::cout);
    }
}//end namespace request_handler