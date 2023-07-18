#pragma once
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "transport_router.h"

#include <sstream>

namespace request_handler {
    class RequestHandler {
    public:
        RequestHandler() = default;
        RequestHandler(transport::catalogue::TransportCatalogue& db, const map_renderer::RenderSettings& render_settings, transport::router::RouterSettings& rs);
        void printResponse(const json::Array& stat_requests);

    private:
        const json::Document responseData(const json::Array& stat_requests);
        
        transport::router::RouterSettings& route_settings_;
        const transport::catalogue::TransportCatalogue& db_;
        const map_renderer::RenderSettings& render_settings_;
    };
}//end namespace request_handler