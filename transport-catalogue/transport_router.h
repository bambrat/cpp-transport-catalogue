#pragma once

#include "router.h"
#include "domain.h"

#include <memory>
#include <variant>

namespace transport::router {
	static const size_t KM = 1000;
	static const size_t HOUR = 60;

	class RouterByGraph {
	public:
		RouterByGraph(const transport::domain::RouterSettings& settings);
		void setRouter(const transport::domain::StopMap& allstops, const transport::domain::BusMap& allbuses);
		
		const std::optional<transport::domain::RouterByStop> getRouterByStop(const transport::domain::Stop* stop) const;
		const std::optional<transport::domain::RouteInfomation> getRouteInfomation(graph::VertexId start, graph::VertexId end) const;

	private:
		void addEdgesByBus(const std::vector<transport::domain::Stop*>::iterator& first, const std::vector<transport::domain::Stop*>::iterator& last, const std::string_view& name);

		const transport::domain::RouterSettings& settings_;

		transport::domain::RouterStopsMap stop_router_map_;
		transport::domain::EdgeIdMap edge_id_map_;

		std::unique_ptr<graph::DirectedWeightedGraph<double>> graph_;
		std::unique_ptr<graph::Router<double>> router_;
	};
}//end namespace transport::router