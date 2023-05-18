#include "transport_catalogue.h"

namespace transport_catalogue {

	void TransportCatalogue::addStop(Stop&& stop) {
		stops.push_back(std::move(stop));
		Stop* stop_ptr = &stops.back();
		stopname_to_stop.insert(StopMap::value_type(stop_ptr->name, stop_ptr));
	}

	void TransportCatalogue::addBus(Bus&& bus) {
		buses.push_back(std::move(bus));
		Bus* bus_ptr = &buses.back();
		busname_to_bus.insert(BusMap::value_type(bus_ptr->name, bus_ptr));

		for (auto & stop : bus_ptr->stops_list) {
			if (stopname_to_stop.find(stop) != stopname_to_stop.end()) {
				bus_ptr->stops.push_back(stopname_to_stop.at(stop));
				stopname_to_stop.at(stop)->buses.push_back(bus_ptr);
				stopname_to_stop.at(stop)->unique_buses.insert(bus_ptr->name);
			}
		}

		bus_ptr->route_length = double(getDistanceBus(bus_ptr));
		bus_ptr->curvature = double(getDistanceBus(bus_ptr) / getLength(bus_ptr));
		bus_ptr->unique_stops = getUniqStops(bus_ptr);
	}

	void TransportCatalogue::addDistance(const Stop* stopA, const Stop* stopB, int dist) {
		stopname_to_stop.at(stopA->name)->next_stop_dist.insert(DistanceMap::value_type(stopB, dist));
	}

	const Bus* TransportCatalogue::getBus(std::string_view name) const {
		if (busname_to_bus.find(name) == busname_to_bus.end()) { return nullptr; }
		return busname_to_bus.at(name);
	}

	const Stop* TransportCatalogue::getStop(std::string_view name) const {
		if (stopname_to_stop.find(name) == stopname_to_stop.end()) { return nullptr; }
		return stopname_to_stop.at(name);
	}

	const std::unordered_set<const Stop*> TransportCatalogue::getUniqStops(const Bus* bus) const {
		std::unordered_set<const Stop*> unique_stops;
		unique_stops.insert(bus->stops.begin(), bus->stops.end());
		return unique_stops;
	}

	double TransportCatalogue::getLength(const Bus* bus) const {
		auto lambComputeDistance = [](const Stop* lhs, const Stop* rhs) {
			return geo::ComputeDistance({ (*lhs).coord.lat, (*lhs).coord.lng },
				{ (*rhs).coord.lat, (*rhs).coord.lng }); };
		return transform_reduce(next(bus->stops.begin()), bus->stops.end(),
			bus->stops.begin(), 0.0, std::plus<>{}, lambComputeDistance);
	}

	size_t TransportCatalogue::getDistanceStop(const Stop* begin, const Stop* finish) const {
		auto distanceStop_frwd = begin->next_stop_dist.count(finish);
		auto distanceStop_back = finish->next_stop_dist.count(begin);

		if (distanceStop_frwd || distanceStop_back) {
			if (distanceStop_frwd)
			{
				return begin->next_stop_dist.find(finish)->second;
			}
			else {
				return finish->next_stop_dist.find(begin)->second;
			}
		}
		return 0;
	}

	size_t TransportCatalogue::getDistanceBus(const Bus* bus) const {
		size_t distanceBus = 0;
		size_t stops_size = bus->stops.size() - 1;
		for (size_t i = 0; i < stops_size; i++) {
			distanceBus += getDistanceStop(bus->stops[i], bus->stops[i + 1]);
		}
		return distanceBus;
	}
}