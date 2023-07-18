#pragma once
#include <vector>
//#include <execution>
#include <unordered_set>
#include <unordered_map>
#include <string_view>
#include <string>
#include <set>

#include "geo.h"

namespace transport::domain {
	struct Bus;
	struct Stop;

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

}//transport::catalogue::domain
