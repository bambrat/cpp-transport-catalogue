#pragma once

#include "domain.h"
#include "json.h"
#include "svg.h"
#include "map_renderer.h"
#include "transport_catalogue.h"


namespace json::reader {
	using namespace json;
	using namespace map_renderer;
	using namespace transport::domain;
	using namespace transport::catalogue;
	
	RenderSettings setRenderSetting(const Dict& render_settings);
	
	void setTransportCatalogueData(TransportCatalogue& catalogue, Array catalogue_data);
}//json::reader