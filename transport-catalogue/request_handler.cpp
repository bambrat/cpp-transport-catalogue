#include "request_handler.h"

namespace request_handler {

    RequestHandler::RequestHandler(transport::catalogue::TransportCatalogue& db, const map_renderer::MapRenderer& map_renderer_) : db_(db), map_renderer_(map_renderer_) {}

    void RequestHandler::printResponseData(const json::Array& stat_requests)
    {
        using namespace std::string_literals;
        using namespace json;

        std::vector<Node> result_request;
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
            }
            else if (type == "Bus") {
                auto& name = rasm.at("name"s).asString();
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
                result = map_renderer_.creatingMapNode(id, db_.getAllBuses(), db_.getAllStops());
            }
            result_request.push_back(result);
        }
        print(Document{ result_request }, std::cout);
    }

}//end namespace request_handler