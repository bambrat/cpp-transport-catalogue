#include "transport_catalogue.h"
#include "request_handler.h"
#include "json_reader.h"

using namespace json::reader;
using namespace transport::catalogue;
using namespace transport::domain;
using namespace json;

int main() {
	TransportCatalogue catalogue;

	auto inputJson = load(std::cin);

	Array base_requests = inputJson.getRoot().asMap().at("base_requests").asArray();

	setTransportCatalogueData(catalogue, base_requests);

	Array stat_requests = inputJson.getRoot().asMap().at("stat_requests").asArray();

	auto& render_settings = inputJson.getRoot().asMap().at("render_settings").asMap();
	
	auto render_dict = setRenderSetting(render_settings);

	map_renderer::MapRenderer map_render(render_dict);

	request_handler::RequestHandler request_handler(catalogue, map_render);

	request_handler.printResponseData(stat_requests);

	return 0;
}