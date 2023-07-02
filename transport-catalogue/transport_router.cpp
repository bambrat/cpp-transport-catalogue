#include "transport_router.h"

namespace transport::router {
	using namespace transport::domain;
	using namespace graph;

	RouterByGraph::RouterByGraph(const RouterSettings& settings) : settings_(settings) {}

	void RouterByGraph::setRouter(const StopMap& allstops, const BusMap& allbuses)
	{
		graph_ = std::make_unique<DirectedWeightedGraph<double>>(2 * allstops.size());

		size_t i = 0;
		for (auto& [name_stop, stop] : allstops) {
			VertexId bus_wait_start = i++;
			VertexId bus_wait_end = i++;

			stop_to_router_[stop] = RouterByStop{ bus_wait_start, bus_wait_end };

			auto id = graph_->addEdge(Edge<double>{bus_wait_start, bus_wait_end, settings_.wait_time});
			edge_id_to_edge_[id] = EdgeByStop{ name_stop, settings_.wait_time };
		}

		for (auto& [name_bus, bus] : allbuses) {
			addEdgesByBus(bus->stops.begin(), bus->stops.end(), name_bus);

			if (!bus->is_roundtrip) {
				auto& a = bus->stops;
				std::reverse(a.begin(), a.end());

				addEdgesByBus(a.begin(), a.end(), name_bus);
			}
		}
		router_ = std::make_unique<Router<double>>(*graph_);
	}

	void RouterByGraph::addEdgesByBus(const std::vector<Stop*>::iterator& first, const std::vector<Stop*>::iterator& last, const std::string_view& name)
	{
		for (auto it = first; it != last; ++it) {

			size_t distance = 0;
			size_t span = 0;
			double velocity = 1.0 / (settings_.velocity * KM / HOUR);

			for (auto it_next = std::next(it); it_next != last; ++it_next) {
				if ((*prev(it_next))->next_stop_dist.count(*it_next))
				{
					distance += (*prev(it_next))->next_stop_dist.find(*it_next)->second;
				}
				else {
					distance += (*it_next)->next_stop_dist.find(*prev(it_next))->second;
				}

				++span;

				VertexId from = stop_to_router_.at(*it).bus_wait_end;
				VertexId to = stop_to_router_.at(*it_next).bus_wait_start;
				auto weight = distance * velocity;

				size_t id = graph_->addEdge(Edge<double>{from, to, weight});
				edge_id_to_edge_[id] = EdgeByBus{ name, span, weight };
			}
		}
	}

	std::optional<RouterByStop> RouterByGraph::getRouterByStop(const Stop* stop) const {
		if (stop_to_router_.count(stop)) { return stop_to_router_.at(stop); }
		return std::nullopt;
	}

	std::optional<RouteInfomation> RouterByGraph::getRouteInfomation(VertexId start, VertexId end) const {
		const auto& route_info = router_->buildRoute(start, end);
		if (route_info) {
			RouteInfomation result;
			result.total_time = route_info->weight;

			for (const auto edge : route_info->edges) {
				result.edges.emplace_back(edge_id_to_edge_.at(edge));
			}
			return result;
		}
		return std::nullopt;
	}
}// end namespace transport::router