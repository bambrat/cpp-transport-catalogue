#include "request_handler.h"
#include "json_reader.h"
#include "serialization.h"
#include "transport_router.h"

#include <fstream>
#include <iostream>
#include <string_view>

using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
	stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		PrintUsage();
		return 1;
	}
	const std::string_view mode(argv[1]);
	json::Document input_data = json::load(std::cin);
	auto& file = input_data.getRoot().asDict().at("serialization_settings").asDict().at("file"s).asString();

	transport::catalogue::TransportCatalogue catalogue;
	if (mode == "make_base"sv) {
		auto& base_requests = input_data.getRoot().asDict().at("base_requests").asArray();
		auto render_settings = json::reader::setRenderSetting(input_data.getRoot().asDict().at("render_settings").asDict());
		auto routing_settings = json::reader::setRouterSetting(input_data.getRoot().asDict().at("routing_settings").asDict());

		json::reader::setTransportData(catalogue, base_requests);
		tc_protobuf::saveData(catalogue, render_settings, routing_settings, file);
	}
	else if (mode == "process_requests"sv) {
		auto& stat_requests = input_data.getRoot().asDict().at("stat_requests").asArray();
		
		auto load_data = tc_protobuf::protoParseFromIstream(file);
		auto render_settings = tc_protobuf::loadRenderSettings(load_data);
		auto routing_settings = tc_protobuf::loadRouterSettings(load_data);

		catalogue = tc_protobuf::loadCatalogueData(load_data);
		map_renderer::MapRenderer map_render(render_settings);
		transport::router::RouterByGraph router_by_graph(routing_settings);

		request_handler::RequestHandler request_handler(catalogue, map_render, router_by_graph);
		request_handler.printResponse(stat_requests);
	}
	else {
		PrintUsage();
		return 1;
	}
}