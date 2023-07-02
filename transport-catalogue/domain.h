#pragma once
#include <vector>
#include <execution>
#include <unordered_set>
#include <unordered_map>
#include <string_view>
#include <set>
#include <variant>

#include "geo.h"
#include "graph.h"
#include "json_builder.h"
#include "svg.h"


namespace transport::domain {
	struct Bus;
	struct Stop;
	struct EdgeId;

	namespace detail {
		struct DistanceHasher {
			std::hash<const void*> hasher;

			std::size_t operator()(const Stop* stop) const noexcept {
				auto hash_1 = static_cast<const void*>(stop);
				return hasher(hash_1);
			}
		};
	}//transport::catalogue::detail

	using DistanceMap = std::unordered_map<const Stop*, size_t, detail::DistanceHasher>;
	using StopMap = std::unordered_map<std::string_view, Stop*>;
	using BusMap = std::unordered_map<std::string_view, Bus*>;

	struct Stop {
		std::string name;
		geo::Coordinates coord = { 0,0 };
		std::vector<Bus*> buses;
		std::set<std::string_view> unique_buses;
		DistanceMap next_stop_dist;
	};

	struct Bus {
		std::string name;
		std::vector<Stop*> stops;
		std::vector<std::string> stops_list;
		std::unordered_set<const Stop*> unique_stops;
		double route_length = 0;
		double curvature = 0;
		bool is_roundtrip = false;
	};

	struct EdgeByStop {
		std::string_view name;
		double time = 0;
	};

	struct EdgeByBus {
		std::string_view bus_name;
		size_t span_count = 0;
		double time = 0;
	};

	struct RouterSettings {
		double wait_time;
		double velocity;
	};

	struct RouterByStop {
		graph::VertexId bus_wait_start;
		graph::VertexId bus_wait_end;
	};

	struct RouteInfomation {
		double total_time = 0.0;
		std::vector<std::variant<domain::EdgeByStop, domain::EdgeByBus>> edges;
	};

	struct EdgeInfo {
		json::Node operator()(const domain::EdgeByStop& edge_info) {
			using namespace std::literals;

			return json::Builder{}.startDict()
				.key("type"s).value("Wait"s)
				.key("stop_name"s).value(std::string(edge_info.name))
				.key("time"s).value(edge_info.time)
				.endDict()
				.build();
		}

		json::Node operator()(const domain::EdgeByBus& edge_info) {
			using namespace std::literals;

			return json::Builder{}.startDict()
				.key("type"s).value("Bus"s)
				.key("bus"s).value(std::string(edge_info.bus_name))
				.key("span_count"s).value(static_cast<int>(edge_info.span_count))
				.key("time"s).value(edge_info.time)
				.endDict()
				.build();
		}
	};

	struct RenderSettings {
		double width_;
		double height_;
		double padding_;
		double line_width_;
		double stop_radius_;
		int bus_label_font_size_;
		std::pair<double, double> bus_label_offset_;
		int stop_label_font_size_;
		std::pair<double, double>  stop_label_offset_;
		svg::Color underlayer_color_;
		double underlayer_width_;
		std::vector<svg::Color> color_palette_;
	};

	using EdgeIdMap = std::unordered_map<graph::EdgeId, std::variant<transport::domain::EdgeByStop, transport::domain::EdgeByBus>>;
	using RouterStopsMap = std::unordered_map<const transport::domain::Stop*, transport::domain::RouterByStop>;

}//transport::catalogue::domain
