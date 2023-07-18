#include "json_reader.h"

#include <iomanip>
#include <vector>
#include <iostream>
#include <cassert>
#include <sstream>
#include <string>

namespace json::reader {
	using namespace transport::domain;
	using namespace transport::catalogue;
	using namespace transport::router;
	using namespace map_renderer;
	using namespace std::literals;
	using namespace json;
	using namespace svg;

	Color setColor(const Node& node) {
		if (node.isString()) {
			return Color{ node.asString() };
		}
		if (node.isArray()) {
			const auto& node_array = node.asArray();
			size_t size = node_array.size();
			if (size == 4) {
				return Rgba{
					static_cast<uint8_t>(node_array[0].asInt()),
						static_cast<uint8_t>(node_array[1].asInt()),
						static_cast<uint8_t>(node_array[2].asInt()),
						node_array[3].asDouble()
				};

			}
			if (size == 3) {
				return Rgb{
					static_cast<uint8_t>(node_array[0].asInt()),
						static_cast<uint8_t>(node_array[1].asInt()),
						static_cast<uint8_t>(node_array[2].asInt())
				};
			}
		}
		else {
			throw  std::logic_error("unknown node"s);
		}
		return Color{};
	}

	RenderSettings setRenderSetting(const Dict& render_settings) {
		RenderSettings result;

		result.width_ = render_settings.at("width"s).asDouble();
		result.height_ = render_settings.at("height"s).asDouble();
		result.padding_ = render_settings.at("padding"s).asDouble();
		result.stop_radius_ = render_settings.at("stop_radius"s).asDouble();
		result.line_width_ = render_settings.at("line_width"s).asDouble();
		result.bus_label_font_size_ = render_settings.at("bus_label_font_size"s).asInt();
		result.underlayer_width_ = render_settings.at("underlayer_width"s).asDouble();
		result.stop_label_font_size_ = render_settings.at("stop_label_font_size"s).asInt();

		const Array& tmp_bus_label_offset = render_settings.at("bus_label_offset"s).asArray();
		result.bus_label_offset_.first = tmp_bus_label_offset[0].asDouble();
		result.bus_label_offset_.second = tmp_bus_label_offset[1].asDouble();

		const Array& tmp_stop_label_offset = render_settings.at("stop_label_offset"s).asArray();
		result.stop_label_offset_.first = tmp_stop_label_offset[0].asDouble();
		result.stop_label_offset_.second = tmp_stop_label_offset[1].asDouble();

		result.underlayer_color_ = setColor(render_settings.at("underlayer_color"s));

		for (const auto& color : render_settings.at("color_palette"s).asArray()) {
			result.color_palette_.push_back(setColor(color));
		}
		return result;
	}

	RouterSettings setRouterSetting(const Dict& routing_settings) {
		RouterSettings res{};
		res.velocity = routing_settings.at("bus_velocity").asDouble();
		res.wait_time = routing_settings.at("bus_wait_time").asDouble();
		return res;
	}

	void setTransportData(TransportCatalogue& catalogue, const Array& base_requests)
	
	{
		using namespace std::literals;
		std::vector<Node> stop_req;
		std::vector<Node> bus_req;

		for (const auto& request : base_requests) {
			auto& type = request.asDict().at("type"s).asString();
			if (type == "Stop"s) {
				stop_req.emplace_back(request);
			}
			if (type == "Bus"s) {
				bus_req.emplace_back(request);
			}
		}

		for (auto & stop_ : stop_req) {
			Stop stop;
			stop.name = stop_.asDict().at("name"s).asString();
			stop.coord = { stop_.asDict().at("latitude"s).asDouble(),
						   stop_.asDict().at("longitude"s).asDouble() };
			catalogue.addStop(std::move(stop));
		}

		for (auto& stop_ : stop_req) {
			auto stopA = catalogue.getStop(stop_.asDict().at("name"s).asString());
			for (const auto& [stop_name, distance] : stop_.asDict().at("road_distances"s).asDict()) {
				auto stopB = catalogue.getStop(stop_name);
				auto distanceAB = distance.asInt();
				catalogue.addDistance(stopA, stopB, distanceAB);
			}
		}

		for (auto& bus_ : bus_req) {
			Bus bus;
			bus.name = bus_.asDict().at("name"s).asString();
			if (!bus_.asDict().at("is_roundtrip"s).asBool()) {
				
				auto bus_route_ring = bus_.asDict().at("stops"s).asArray();
				auto& bus_route_line = bus_route_ring;

				for (int i = int(bus_route_ring.size() - 2); i > -1; --i) {
					bus_route_line.push_back(bus_route_ring.at(i));
				}

				for (auto& stop_name : bus_route_line) {
					bus.stops_list.push_back(stop_name.asString());
				}
			}
			else {
				for (const auto& stop_name : bus_.asDict().at("stops"s).asArray()) {
					bus.stops_list.push_back(stop_name.asString());
				}
				bus.is_roundtrip = true;
			}
			catalogue.addBus(std::move(bus));
		}

		//router_by_graph.setRouter(catalogue.getAllStops(), catalogue.getAllBuses());

	}
}//end namespace json::reader