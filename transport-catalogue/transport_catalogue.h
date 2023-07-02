#pragma once
#include "domain.h"
#include <deque>

namespace transport::catalogue {
	class TransportCatalogue {
				
	public:
		TransportCatalogue() = default;

		void addStop(transport::domain::Stop&& stop);
		void addBus(transport::domain::Bus&& bus);
		void addDistance(const transport::domain::Stop* stopA, const transport::domain::Stop* stopB, int dist);

		const transport::domain::Stop* getStop(std::string_view name) const;
		const transport::domain::StopMap& getAllStops() const;
		const transport::domain::Bus* getBus(std::string_view name) const;
		const transport::domain::BusMap& getAllBuses() const;

	private:
		double getLength(const transport::domain::Bus* bus) const;
		size_t getDistanceStop(const transport::domain::Stop* start, const transport::domain::Stop* finish) const;
		size_t getDistanceBus(const transport::domain::Bus* bus) const;
		const std::unordered_set<const transport::domain::Stop*> getUniqStops(const transport::domain::Bus* bus) const;

		std::deque<transport::domain::Stop> stops;
		transport::domain::StopMap stopname_to_stop;

		std::deque<transport::domain::Bus> buses;
		transport::domain::BusMap busname_to_bus;
	};
}//end namespace transport::catalogue