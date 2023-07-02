#pragma once
#include "domain.h"
#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "transport_router.h"

namespace json::reader {
	using namespace transport;

	domain::RenderSettings setRenderSetting(const Dict& render_settings);
	domain::RouterSettings setRouterSetting(const Dict& routing_settings);
	void setTransportData(catalogue::TransportCatalogue& catalogue, router::RouterByGraph& router_by_graph , const json::Array& catalogue_data);

}//end namespace json::reader