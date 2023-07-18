#include "request_handler.h"
#include "serialization.h"

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
	if (mode == "make_base"sv) {
		tc_protobuf::Serialization serialization(input_data, mode);

	}
	else if (mode == "process_requests"sv) {
		tc_protobuf::Serialization serialization(input_data, mode);
		transport::catalogue::TransportCatalogue catalogue = serialization.composeTransportCatalogue();

		auto render_settings = serialization.getRenderSettings();
		auto routing_settings = serialization.getRouterSettings();
		request_handler::RequestHandler handler(catalogue, render_settings, routing_settings);

		auto stat_requests = serialization.getStatRequests();
		handler.printResponse(stat_requests);

	}
	else {
		PrintUsage();
		return 1;
	}
}