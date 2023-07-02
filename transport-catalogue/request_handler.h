#pragma once
#include "map_renderer.h"
#include "json.h"
#include "transport_catalogue.h"
#include "transport_router.h"
#include "domain.h"

#include <sstream>

namespace request_handler {
    class RequestHandler {
    public:
        RequestHandler(transport::catalogue::TransportCatalogue& db, const map_renderer::MapRenderer& map_renderer, transport::router::RouterByGraph& router_by_graph);
        void printResponse(const json::Array& stat_requests);

    private:
        const json::Document responseData(const json::Array& stat_requests);

        const transport::catalogue::TransportCatalogue& db_;
        const map_renderer::MapRenderer& map_renderer_;
        const transport::router::RouterByGraph& router_by_graph_;
    };
}//end namespace request_handler