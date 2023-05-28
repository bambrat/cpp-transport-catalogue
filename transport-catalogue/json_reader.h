#pragma once
#include "domain.h"
#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"

namespace json::reader {
	map_renderer::RenderSettings setRenderSetting(const Dict& render_settings);
	void setTransportCatalogueData(transport::catalogue::TransportCatalogue& catalogue,const json::Array& catalogue_data);
}//json::reader