#pragma once
#include "map_renderer.h"
#include "json.h"

#include <sstream>
#include "transport_catalogue.h"

namespace request_handler {
    using namespace transport::catalogue;
    using namespace map_renderer;
    using namespace json;

    class RequestHandler {
    public:
        RequestHandler(TransportCatalogue& db, const MapRenderer& map_renderer_);
        Dict creatingMapNode(int id) const;
        void printResponseData(Array stat_requests);

    private:
        TransportCatalogue& db_;
        const MapRenderer& map_renderer_;
    };

}//end namespace request_handler