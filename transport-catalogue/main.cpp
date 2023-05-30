#include "transport_catalogue.h"
#include "request_handler.h"
#include "json_reader.h"

using namespace json::reader;
using namespace transport::catalogue;
using namespace transport::domain;
using namespace json;
<<<<<<< HEAD
 
=======

>>>>>>> 10ab7554ddb88db1f9df3aa1be3851c045b11d50
int main() {
	TransportCatalogue catalogue;

	auto inputJson = load(std::cin);

<<<<<<< HEAD
	Array base_requests = inputJson.getRoot().asDict().at("base_requests").asArray();

	setTransportCatalogueData(catalogue, base_requests);

	Array stat_requests = inputJson.getRoot().asDict().at("stat_requests").asArray();

	auto& render_settings = inputJson.getRoot().asDict().at("render_settings").asDict();

=======
	Array base_requests = inputJson.getRoot().asMap().at("base_requests").asArray();

	setTransportCatalogueData(catalogue, base_requests);

	Array stat_requests = inputJson.getRoot().asMap().at("stat_requests").asArray();

	auto& render_settings = inputJson.getRoot().asMap().at("render_settings").asMap();
	
>>>>>>> 10ab7554ddb88db1f9df3aa1be3851c045b11d50
	auto render_dict = setRenderSetting(render_settings);

	map_renderer::MapRenderer map_render(render_dict);

	request_handler::RequestHandler request_handler(catalogue, map_render);

	request_handler.printResponseData(stat_requests);

	return 0;
}