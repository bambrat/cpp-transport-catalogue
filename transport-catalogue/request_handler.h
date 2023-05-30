#pragma once
#include "map_renderer.h"
#include "json.h"
<<<<<<< HEAD
#include "transport_catalogue.h"
#include "json_builder.h"

#include <sstream>

=======

#include <sstream>
#include "transport_catalogue.h"
>>>>>>> 10ab7554ddb88db1f9df3aa1be3851c045b11d50

namespace request_handler {
    class RequestHandler {
    public:
        RequestHandler(transport::catalogue::TransportCatalogue& db, const map_renderer::MapRenderer& map_renderer_);
        void printResponseData(const json::Array& stat_requests);
<<<<<<< HEAD
=======

>>>>>>> 10ab7554ddb88db1f9df3aa1be3851c045b11d50
    private:
        transport::catalogue::TransportCatalogue& db_;
        const map_renderer::MapRenderer& map_renderer_;
    };
}//end namespace request_handler