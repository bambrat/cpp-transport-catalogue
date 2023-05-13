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

		for (auto stop : bus_ptr->stops) {
			stop->buses.push_back(bus_ptr);
		}
		bus_ptr->route_length = getDistanceBus(bus_ptr);
	}

	void TransportCatalogue::addDistance(const std::vector<Distance>& distances) {
		for (auto& distance : distances) {
			distance_to_stop.insert(DistanceMap::value_type({ distance.A, distance.B }, distance.dist));
		}
	}

	Bus* TransportCatalogue::getBus(std::string_view name) {
		if (busname_to_bus.find(name) == busname_to_bus.end()) { return nullptr; }
		return busname_to_bus.at(name);
	}

	Stop* TransportCatalogue::getStop(std::string_view name) {
		if (stopname_to_stop.find(name) == stopname_to_stop.end()) { return nullptr; }
		return stopname_to_stop.at(name);
	}

	std::unordered_set<const Stop*> TransportCatalogue::getUniqStops(Bus* bus) {
		std::unordered_set<const Stop*> unique_stops;
		unique_stops.insert(bus->stops.begin(), bus->stops.end());
		return unique_stops;
	}

	std::unordered_set<const Bus*> TransportCatalogue::getUniqBuses(Stop* stop) {
		std::unordered_set<const Bus*> unique_stops;
		unique_stops.insert(stop->buses.begin(), stop->buses.end());
		return unique_stops;
	}

	double TransportCatalogue::getLength(Bus* bus) {
		auto lambComputeDistance = [](const Stop* lhs, const Stop* rhs) {
			return geo::ComputeDistance({ (*lhs).latitude, (*lhs).longitude },
				{ (*rhs).latitude, (*rhs).longitude }); };
		return transform_reduce(next(bus->stops.begin()), bus->stops.end(),
			bus->stops.begin(), 0.0, std::plus<>{}, lambComputeDistance);
	}

	size_t TransportCatalogue::getDistanceStop(const Stop* begin, const Stop* finish) {
		auto distanceStop_frwd{ distance_to_stop.find({ begin, finish }) };
		auto distanceStop_back{ distance_to_stop.find({ finish, begin }) };

		if (distanceStop_frwd != distance_to_stop.end() || distanceStop_back != distance_to_stop.end()) {
			if (distanceStop_frwd != distance_to_stop.end())
			{
				return distanceStop_frwd->second;
			}
			else {
				return distanceStop_back->second;
			}
		}
		return 0;
	}

	size_t TransportCatalogue::getDistanceBus(Bus* bus) {
		size_t distanceBus = 0;
		size_t stops_size = bus->stops.size() - 1;
		for (size_t i = 0; i < stops_size; i++) {
			distanceBus += getDistanceStop(bus->stops[i], bus->stops[i + 1]);
		}
		return distanceBus;
	}
}