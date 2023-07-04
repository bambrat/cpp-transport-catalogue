#include "request_handler.h"
#include "json_reader.h"
#include <cassert>

using namespace transport;

int main() {
	json::Document input_data = json::load(std::cin);

	auto& base_requests = input_data.getRoot().asDict().at("base_requests").asArray();
	auto& stat_requests = input_data.getRoot().asDict().at("stat_requests").asArray();
	auto routing_settings = json::reader::setRouterSetting(input_data.getRoot().asDict().at("routing_settings").asDict());
	auto render_settings = json::reader::setRenderSetting(input_data.getRoot().asDict().at("render_settings").asDict());

	map_renderer::MapRenderer map_render(render_settings);
	catalogue::TransportCatalogue catalogue;
	router::RouterByGraph router_by_graph(routing_settings);

	json::reader::setTransportData(catalogue, router_by_graph, base_requests);
	request_handler::RequestHandler request_handler(catalogue, map_render, router_by_graph);

	request_handler.printResponse(stat_requests);
	
	return 0;
}