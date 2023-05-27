#pragma once
#include "domain.h"
#include <deque>

namespace transport::catalogue {
	using namespace transport::domain;

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