#include "stat_reader.h"

namespace transport_catalogue {
	namespace console_data_processing {
		void showBusData(const TransportCatalogue& catalogue, std::string_view name) {
			auto bus = catalogue.getBus(name);
			
			if (bus == nullptr) {
				std::cout << "Bus " << name << ": not found" << std::endl;
			}
			else {
				std::cout << "Bus " << bus->name << ": "
					<< bus->stops.size() << " stops on route, "
					<< bus->unique_stops.size() << " unique stops, "
					<< bus->route_length << " route length, "
					<< std::setprecision(6) << bus->curvature
					<< " curvature" << std::endl;
			}
		}

		void showStopData(const TransportCatalogue& catalogue, std::string_view name) {
			auto stop = catalogue.getStop(name);

			if (stop == nullptr) {
				std::cout << "Stop " << name << ": not found" << std::endl;
			}
			else {
				//auto unique_buses = catalogue.getUniqBuses(stop);
				auto &unique_buses = stop->unique_buses;

				if (unique_buses.size() == 0) {
					std::cout << "Stop " << name << ": no buses" << std::endl;
				}
				else {
					std::cout << "Stop " << stop->name << ": buses ";
					for (auto& bus : unique_buses) {
						std::cout << bus << " ";
					}
					std::cout << std::endl;
				}
			}
		}

		void readRequest(std::istream& input, const TransportCatalogue& catalogue) {
			using namespace detail;

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
				if (str[0] == BUS_DELIMITER) {
					str = str.substr(BUS_BEGIN_LINE.size(), str.size());
					showBusData(catalogue, str);
				}
				else if (str[0] == STOP_DELIMITER) {
					str = str.substr(STOP_BEGIN_LINE.size(), str.size());
					showStopData(catalogue, str);
				}
				else {
					std::cout << "Error query" << std::endl;
					return;
				}
			}
		}
	} //namespace console_data_processing
}//namespace transport_catalogue