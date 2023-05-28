#pragma once
#include "map_renderer.h"
#include "json.h"

#include <sstream>
#include "transport_catalogue.h"

namespace request_handler {
    class RequestHandler {
    public:
        RequestHandler(transport::catalogue::TransportCatalogue& db, const map_renderer::MapRenderer& map_renderer_);
        void printResponseData(const json::Array& stat_requests);

    private:
        transport::catalogue::TransportCatalogue& db_;
        const map_renderer::MapRenderer& map_renderer_;
    };
}//end namespace request_handler