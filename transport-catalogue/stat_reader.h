#pragma once
#include <iomanip>

#include "input_reader.h"

namespace transport_catalogue {
	namespace bus {
		struct QueryResult {
			std::string_view name;
			bool not_found = true;
			size_t stops_on_route = 0;
			size_t unique_stops = 0;
			double route_length = 0;
			double curvature = 0;
		};

		void dataOutput(const QueryResult data, std::ostream& output);
		QueryResult query(TransportCatalogue& catalogue, std::string_view name);

	}//end namespace bus

	namespace stop {
		struct QueryResult {
			std::string_view name;
			bool not_found = true;
			std::vector <std::string_view> buses_name;
		};

		void dataOutput(const QueryResult& data, std::ostream& output);
		QueryResult query(TransportCatalogue& catalogue, std::string_view name);

	}//namespace stop

	namespace console_data_processing {
		void readRequest(std::istream& input, TransportCatalogue& catalogue);
	}
}