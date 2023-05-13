#pragma once
#include <deque>
#include <vector>
#include <iostream>
#include <execution>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <string_view>

#include "geo.h"

namespace transport_catalogue {

	struct Bus;

	struct Stop {
		std::string name;
		double latitude = 0;
		double longitude = 0;
		std::vector<Bus*> buses;
	};

	struct Bus {
		std::string name;
		std::vector<Stop*> stops;
		size_t route_length = 0;
	};

	struct Distance {
		const Stop* A;
		const Stop* B;
		int dist;
	};

	struct DistanceHasher {
		std::hash<const void*> hasher;

		std::size_t operator()(const std::pair<const Stop*, const Stop*> pair_stops) const noexcept {
			auto hash_1 = static_cast<const void*>(pair_stops.first);
			auto hash_2 = static_cast<const void*>(pair_stops.second);
			return hasher(hash_1) * 43 + hasher(hash_2);
		}
	};

	typedef  std::unordered_map<std::string_view, Stop*> StopMap;
	typedef  std::unordered_map<std::string_view, Bus*> BusMap;
	typedef  std::unordered_map<std::pair<const Stop*, const  Stop*>, size_t, DistanceHasher> DistanceMap;

	class TransportCatalogue {
	public:

		//stop
		void addStop(Stop&& stop);
		Stop* getStop(std::string_view name);
		std::unordered_set<const Stop*> getUniqStops(Bus* bus);

		//bus
		void addBus(Bus&& bus);
		Bus* getBus(std::string_view name);
		std::unordered_set<const Bus*> getUniqBuses(Stop* stop);
		double getLength(Bus* bus);

		//dist
		void addDistance(const std::vector<Distance>& distances);
		size_t getDistanceBus(Bus* bus);
		size_t getDistanceStop(const Stop* start, const Stop* finish);

	private:
		std::deque<Stop> stops;
		StopMap stopname_to_stop;

		std::deque<Bus> buses;
		BusMap busname_to_bus;

		DistanceMap distance_to_stop;
	};
}