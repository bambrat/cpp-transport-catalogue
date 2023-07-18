#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"
#include "json_reader.h"

#include "transport_catalogue.pb.h"
#include "svg.pb.h"
#include "map_renderer.pb.h"
#include "transport_router.pb.h"

#include <iostream>
#include <fstream>

namespace tc_protobuf {
	class Serialization {

	public:
		Serialization(const json::Document& input_data, const std::string_view& mode);

		transport::catalogue::TransportCatalogue composeTransportCatalogue();
		transport::router::RouterSettings getRouterSettings();
		map_renderer::RenderSettings getRenderSettings();
		json::Array getStatRequests();

	private:
		void saveData();
		int getСachePointer(const std::deque<std::string_view>& cache, const std::string_view& find_name);
		svg::Color convertColorProtoToColor(const ColorProto& cp);
		ColorProto convertColorToColorProto(const svg::Color& tc_color);
		tc_protobuf::DataToSaveProto protoParseFromIstream();
		const std::string& getFileName();

		const json::Document& input_data_;
		tc_protobuf::DataToSaveProto proto_data_;
	};
}//end namespace tc_protobuf