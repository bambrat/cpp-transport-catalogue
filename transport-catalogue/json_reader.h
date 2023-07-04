#pragma once
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "transport_router.h"

namespace json::reader {
	map_renderer::RenderSettings setRenderSetting(const Dict& render_settings);
	transport::router::RouterSettings setRouterSetting(const Dict& routing_settings);
	void setTransportData(transport::catalogue::TransportCatalogue& catalogue, transport::router::RouterByGraph& router_by_graph , const json::Array& catalogue_data);
}//end namespace json::reader