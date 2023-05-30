#include "json_reader.h"

#include <iomanip>
#include <vector>
#include <iostream>
#include <cassert>
#include <sstream>
#include <string>

<<<<<<< HEAD
=======

>>>>>>> 10ab7554ddb88db1f9df3aa1be3851c045b11d50
namespace json::reader {

	svg::Color setColor(const Node& node) {
		if (node.isString()) {
			return svg::Color{ node.asString() };
		}
		if (node.isArray()) {
			const auto& node_array = node.asArray();
			size_t size = node_array.size();
			if (size == 4) {
				return svg::Rgba{
					static_cast<uint8_t>(node_array[0].asInt()),
						static_cast<uint8_t>(node_array[1].asInt()),
						static_cast<uint8_t>(node_array[2].asInt()),
						node_array[3].asDouble()
				};

			}
			if (size == 3) {
				return svg::Rgb{
					static_cast<uint8_t>(node_array[0].asInt()),
						static_cast<uint8_t>(node_array[1].asInt()),
						static_cast<uint8_t>(node_array[2].asInt())
				};
			}
		}
		else {
			throw  std::logic_error("unknown node");
		}
		return svg::Color{};
	}

	map_renderer::RenderSettings setRenderSetting(const Dict& render_settings) {
		using namespace map_renderer;
		using namespace std::literals;
		RenderSettings result;

		result.width_ = render_settings.at("width"s).asDouble();
		result.height_ = render_settings.at("height"s).asDouble();
		result.padding_ = render_settings.at("padding"s).asDouble();
		result.stop_radius_ = render_settings.at("stop_radius"s).asDouble();
		result.line_width_ = render_settings.at("line_width"s).asDouble();
		result.bus_label_font_size_ = render_settings.at("bus_label_font_size"s).asInt();
		result.underlayer_width_ = render_settings.at("underlayer_width"s).asDouble();
		result.stop_label_font_size_ = render_settings.at("stop_label_font_size"s).asInt();

		const json::Array& tmp_bus_label_offset = render_settings.at("bus_label_offset"s).asArray();
		result.bus_label_offset_.first = tmp_bus_label_offset[0].asDouble();
		result.bus_label_offset_.second = tmp_bus_label_offset[1].asDouble();

		const json::Array& tmp_stop_label_offset = render_settings.at("stop_label_offset"s).asArray();
		result.stop_label_offset_.first = tmp_stop_label_offset[0].asDouble();
		result.stop_label_offset_.second = tmp_stop_label_offset[1].asDouble();

		result.underlayer_color_ = setColor(render_settings.at("underlayer_color"s));

		for (const auto& color : render_settings.at("color_palette"s).asArray()) {
			result.color_palette_.push_back(setColor(color));
		}
		return result;
	}

	void setTransportCatalogueData(transport::catalogue::TransportCatalogue& catalogue, const json::Array& catalogue_data)
	{
		using namespace std::literals;
		using namespace transport::domain;

<<<<<<< HEAD
=======

>>>>>>> 10ab7554ddb88db1f9df3aa1be3851c045b11d50
		std::vector<Node> stop_req;
		std::vector<Node> bus_req;

		for (const auto& request : catalogue_data) {
<<<<<<< HEAD
			const std::string& type = request.asDict().at("type"s).asString();
=======
			const std::string& type = request.asMap().at("type"s).asString();
>>>>>>> 10ab7554ddb88db1f9df3aa1be3851c045b11d50
			if (type == "Stop"s) {
				stop_req.push_back(request);
			}
			if (type == "Bus"s) {
				bus_req.push_back(request);
			}
		}

		for (auto & stop_ : stop_req) {
			Stop stop;
<<<<<<< HEAD
			stop.name = stop_.asDict().at("name"s).asString();
			stop.coord = { stop_.asDict().at("latitude"s).asDouble(),
						   stop_.asDict().at("longitude"s).asDouble() };
=======
			stop.name = stop_.asMap().at("name"s).asString();
			stop.coord = { stop_.asMap().at("latitude"s).asDouble(), 
						   stop_.asMap().at("longitude"s).asDouble() };
>>>>>>> 10ab7554ddb88db1f9df3aa1be3851c045b11d50
			catalogue.addStop(std::move(stop));
		}

		for (auto& stop_ : stop_req) {
<<<<<<< HEAD
			auto stopA = catalogue.getStop(stop_.asDict().at("name"s).asString());
			for (const auto& [stop_name, distance] : stop_.asDict().at("road_distances"s).asDict()) {
=======
			auto stopA = catalogue.getStop(stop_.asMap().at("name"s).asString());
			for (const auto& [stop_name, distance] : stop_.asMap().at("road_distances"s).asMap()) {
>>>>>>> 10ab7554ddb88db1f9df3aa1be3851c045b11d50
				auto stopB = catalogue.getStop(stop_name);
				auto distanceAB = distance.asInt();
				catalogue.addDistance(stopA, stopB, distanceAB);
			}
		}

		for (auto& bus_ : bus_req) {
			Bus bus;
<<<<<<< HEAD
			bus.name = bus_.asDict().at("name"s).asString();
			if (!bus_.asDict().at("is_roundtrip"s).asBool()) {
				
				auto bus_route_ring = bus_.asDict().at("stops"s).asArray();
=======
			bus.name = bus_.asMap().at("name"s).asString();
			if (!bus_.asMap().at("is_roundtrip"s).asBool()) {
				
				auto bus_route_ring = bus_.asMap().at("stops"s).asArray();
>>>>>>> 10ab7554ddb88db1f9df3aa1be3851c045b11d50
				auto& bus_route_line = bus_route_ring;

				for (int i = int(bus_route_ring.size() - 2); i > -1; --i) {
					bus_route_line.push_back(bus_route_ring.at(i));
				}

				for (auto& stop_name : bus_route_line) {
					bus.stops_list.push_back(stop_name.asString());
				}
			}
			else {
<<<<<<< HEAD
				for (const auto& stop_name : bus_.asDict().at("stops"s).asArray()) {
=======
				for (const auto& stop_name : bus_.asMap().at("stops"s).asArray()) {
>>>>>>> 10ab7554ddb88db1f9df3aa1be3851c045b11d50
					bus.stops_list.push_back(stop_name.asString());
				}
				bus.is_roundtrip = true;
			}
			catalogue.addBus(std::move(bus));
		}
	}
}//json::reader