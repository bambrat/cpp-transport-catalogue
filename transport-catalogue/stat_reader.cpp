#include "stat_reader.h"

namespace transport_catalogue {
	namespace bus {
		void dataOutput(const QueryResult data, std::ostream& output) {
			if (data.not_found) {
				output << "Bus " << data.name << ": not found" << std::endl;
			}
			else {
				output << "Bus " << data.name << ": "
					<< data.stops_on_route << " stops on route, "
					<< data.unique_stops << " unique stops, "
					<< data.route_length << " route length, "
					<< std::setprecision(6) << data.curvature
					<< " curvature" << std::endl;
			}
		}
		QueryResult query(TransportCatalogue& catalogue, std::string_view name) {
			QueryResult bus_info;

			auto bus = catalogue.getBus(name);
			if (bus != nullptr) {
				bus_info.name = bus->name;
				bus_info.not_found = false;
				bus_info.stops_on_route = bus->stops.size();
				bus_info.unique_stops = catalogue.getUniqStops(bus).size();
				bus_info.route_length = double(bus->route_length);
				bus_info.curvature = double(catalogue.getDistanceBus(bus) / catalogue.getLength(bus));
			}
			else {
				bus_info.name = name;
				bus_info.not_found = true;
			}
			return bus_info;
		}
	}//end namespace bus

	namespace stop {
		void dataOutput(const QueryResult& data, std::ostream& output) {
			if (data.not_found) {
				output << "Stop " << data.name << ": not found" << std::endl;
			}
			else {
				if (data.buses_name.size() == 0) {
					output << "Stop " << data.name << ": no buses" << std::endl;
				}
				else {
					output << "Stop " << data.name << ": buses ";

					for (std::string_view bus_name : data.buses_name) {
						output << bus_name;
						output << " ";
					}
					output << std::endl;
				}
			}
		}
		QueryResult query(TransportCatalogue& catalogue, std::string_view name) {
			std::unordered_set<const Bus*> unique_buses;
			QueryResult stop_info;

			auto stop = catalogue.getStop(name);

			if (stop != nullptr) {
				stop_info.name = stop->name;
				stop_info.not_found = false;
				unique_buses = catalogue.getUniqBuses(stop);

				if (unique_buses.size() > 0) {
					for (const Bus* bus : unique_buses) {
						stop_info.buses_name.push_back(bus->name);
					}
					std::sort(stop_info.buses_name.begin(), stop_info.buses_name.end());
				}
			}
			else {
				stop_info.name = name;
				stop_info.not_found = true;
			}
			return stop_info;
		}
	}//namespace stop
	namespace console_data_processing {
		void readRequest(std::istream& input, TransportCatalogue& catalogue) {
			int count = 0;
			input >> count;

			if (!count) return;

			std::string line;
			std::vector<std::string> query;

			for (int i = 0; i < count; ++i) {
				std::getline(input >> std::ws, line);
				query.push_back(line);
			}

			for (auto& str : query) {
				if (str[0] == bus::DELIMITER) {
					str = str.substr(bus::BEGIN_LINE.size(), str.size());
					bus::dataOutput(bus::query(catalogue, str), std::cout);
				}
				else if (str[0] == stop::DELIMITER) {
					str = str.substr(stop::BEGIN_LINE.size(), str.size());
					stop::dataOutput(stop::query(catalogue, str), std::cout);
				}
				else {
					std::cout << "Error query" << std::endl;
					return;
				}
			}
		}
	} //namespace console_data_processing
}//namespace transport_catalogue