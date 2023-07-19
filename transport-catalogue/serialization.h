#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"

#include "transport_catalogue.pb.h"
#include "svg.pb.h"
#include "map_renderer.pb.h"
#include "transport_router.pb.h"

#include <iostream>
#include <fstream>

namespace tc_protobuf {
	void saveData(transport::catalogue::TransportCatalogue& db, map_renderer::RenderSettings& rs, transport::router::RouterSettings& routs, const std::string& file);
	tc_protobuf::DataToSaveProto protoParseFromIstream(std::string file);
	transport::catalogue::TransportCatalogue loadCatalogueData(tc_protobuf::DataToSaveProto proto);
	map_renderer::RenderSettings loadRenderSettings(tc_protobuf::DataToSaveProto proto);
	transport::router::RouterSettings loadRouterSettings(tc_protobuf::DataToSaveProto proto);
}//end namespace tc_protobuf