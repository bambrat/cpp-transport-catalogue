#pragma once
#include "domain.h"
#include <deque>

<<<<<<< HEAD
namespace transport::catalogue {
	using namespace transport::domain;
=======
#include "geo.h"

namespace transport_catalogue {
	struct Bus;
	struct Stop;

	namespace detail {
		struct DistanceHasher {
			std::hash<const void*> hasher;

			std::size_t operator()(const transport_catalogue::Stop* stop) const noexcept {
				auto hash_1 = static_cast<const void*>(stop);
				return hasher(hash_1);
			}
		};
	}

	using DistanceMap = std::unordered_map<const Stop*, size_t, detail::DistanceHasher>;

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
		std::vector<std::string_view> stops_list;
		std::unordered_set<const Stop*> unique_stops;
		double route_length = 0;
		double curvature = 0;
	};

	using StopMap = std::unordered_map<std::string_view, Stop*> ;
	using BusMap = std::unordered_map<std::string_view, Bus*> ;
>>>>>>> 9a9d6c44286eb5ed1a00575106dff951d2ace6dc

	class TransportCatalogue {
		
	public:
		TransportCatalogue() = default;

		void addStop(Stop&& stop);
		void addBus(Bus&& bus);
		void addDistance(const Stop* stopA, const Stop* stopB, int dist);

		const Stop* getStop(std::string_view name) const;
		const Bus* getBus(std::string_view name) const;
		const BusMap getAllBuses() const;
		const StopMap getAllStops() const;

		const std::unordered_set<const Stop*> getUniqStops(const Bus* bus) const;
		size_t getDistanceStop(const Stop* start, const Stop* finish) const;
		size_t getDistanceBus(const Bus* bus) const;
		double getLength(const Bus* bus) const;

	private:
		std::deque<Stop> stops;
		StopMap stopname_to_stop;

		std::deque<Bus> buses;
		BusMap busname_to_bus;
	};
}//transport::catalogue