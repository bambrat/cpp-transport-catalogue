#include "request_handler.h"

namespace request_handler {

    RequestHandler::RequestHandler(transport::catalogue::TransportCatalogue& db, const map_renderer::MapRenderer& map_renderer_) : db_(db), map_renderer_(map_renderer_) {}

    void RequestHandler::printResponseData(const json::Array& stat_requests)
    {
        using namespace std::string_literals;
        using namespace json;

        std::vector<Node> result_request;
<<<<<<< HEAD
        
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
                
=======
        for (auto& request : stat_requests) {
            auto& rasm = request.asMap();
            const std::string& type = rasm.at("type"s).asString();
            auto id = rasm.at("id"s).asInt();
            std::string not_fount = "not found";

            Dict result;
            if (type == "Stop"s) {
                auto& name = rasm.at("name"s).asString();
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
>>>>>>> 10ab7554ddb88db1f9df3aa1be3851c045b11d50
            }
            else if (type == "Bus") {
                auto& name = rasm.at("name"s).asString();
                auto bus = db_.getBus(name);

<<<<<<< HEAD
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
=======
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
                result = map_renderer_.creatingMapNode(id, db_.getAllBuses(), db_.getAllStops());
            }
            result_request.push_back(result);
        }
        print(Document{ result_request }, std::cout);
    }

>>>>>>> 10ab7554ddb88db1f9df3aa1be3851c045b11d50
}//end namespace request_handler