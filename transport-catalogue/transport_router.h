#pragma once

#include "router.h"
#include "domain.h"
#include "json_builder.h"

#include <memory>
#include <variant>

namespace transport::router {
	static const size_t KM = 1000;
	static const size_t HOUR = 60;

	struct EdgeByStop {
		std::string_view name;
		double time = 0;
	};

	struct EdgeByBus {
		std::string_view bus_name;
		int span_count = 0;
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
		std::vector<std::variant<EdgeByStop, EdgeByBus>> edges;
	};

	struct EdgeInfo {
		json::Node operator()(const EdgeByStop& edge_info) {
			using namespace std::literals;

			return json::Builder{}.startDict()
				.key("type"s).value("Wait"s)
				.key("stop_name"s).value(std::string(edge_info.name))
				.key("time"s).value(edge_info.time)
				.endDict()
				.build();
		}

		json::Node operator()(const EdgeByBus& edge_info) {
			using namespace std::literals;

			return json::Builder{}.startDict()
				.key("type"s).value("Bus"s)
				.key("bus"s).value(std::string(edge_info.bus_name))
				.key("span_count"s).value(edge_info.span_count)
				.key("time"s).value(edge_info.time)
				.endDict()
				.build();
		}
	};

	using EdgeIdMap = std::unordered_map<graph::EdgeId, std::variant<EdgeByStop, EdgeByBus>>;
	using RouterStopsMap = std::unordered_map<const transport::domain::Stop*, RouterByStop>;

	class RouterByGraph {
	public:
		RouterByGraph(const RouterSettings& settings);
		void setRouter(const transport::domain::StopMap& allstops, const transport::domain::BusMap& allbuses);
		
		const std::optional<RouterByStop> getRouterByStop(const transport::domain::Stop* stop) const;
		std::optional<RouteInfomation> getRouteInfomation(const transport::domain::Stop* from, const transport::domain::Stop* to) const;

	private:
		void addEdgesByBus(const std::vector<transport::domain::Stop*>::iterator& first, const std::vector<transport::domain::Stop*>::iterator& last, const std::string_view& name);

		const RouterSettings& settings_;

		RouterStopsMap stop_router_map_;
		EdgeIdMap edge_id_map_;

		std::unique_ptr<graph::DirectedWeightedGraph<double>> graph_;
		std::unique_ptr<graph::Router<double>> router_;
	};
}//end namespace transport::router