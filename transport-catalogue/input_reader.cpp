#include "input_reader.h"

namespace transport_catalogue {

	namespace detail {
		//преобразует строку в double
		double stringToDouble(const std::string& s)
		{
			std::istringstream i(s);
			double x;
			if (!(i >> x))
				return 0;
			return x;
		}

		//убирает все пробелы вначале и в конце строки
		std::string_view cutWord(std::string_view str, const char c = CUT_DEFAULT) {
			if (str.find_first_not_of(c)) {
				str = str.substr(str.find_first_not_of(c), str.size());
			}
			if (str.find_last_not_of(c)) {
				str = str.substr(0, str.find_last_not_of(c) + 1);
			}
			if (str.size() == 2 && str[1] == c) {
				str = str.substr(0, 1);
			}
			return str;
		}

		//разбиваем строку подстрокой ( подстрока, строка ) - возвращаем вектор sv
		std::vector<std::string_view > SplitString(const std::string_view sv, const std::string splitter = SPLIT_DEFAULT) {
			size_t pos_start = 0, pos_end, delim_len = splitter.length();
			std::string_view token;
			std::vector<std::string_view> res;

			while ((pos_end = sv.find(splitter, pos_start)) != std::string_view::npos) {
				token = sv.substr(pos_start, pos_end - pos_start);
				pos_start = pos_end + delim_len;
				res.push_back(token);
			}

			res.push_back(sv.substr(pos_start));

			return res;
		}
	} //namespace detail

	namespace stop {
		Stop processing(const std::string_view record) {
			using namespace detail;
			Stop stop;
			auto _ = SplitString(record, MAIN_SPLIT);
			stop.name = cutWord(_.at(0));

			auto coords = SplitString(_.at(1));
			stop.latitude = stringToDouble(std::string(coords.at(0)));
			stop.longitude = stringToDouble(std::string(coords.at(1)));
			return stop;
		}
	}//namespace stop
	namespace distance {
		std::vector <Distance > processing(const std::string_view record, TransportCatalogue& catalogue) {
			using namespace detail;
			auto rec = SplitString(record);

			//если нет данных о дистанции
			if (rec.size() < MIN_VECTOR_SIZE) return {};

			std::vector<Distance> distances{};
			auto stopA = catalogue.getStop(SplitString(record, MAIN_SPLIT).at(0));

			for (int i = 2; i < rec.size(); ++i) {
				auto _ = SplitString(rec.at(i), DELIMITER);
				auto stopB = catalogue.getStop(_.at(1));
				auto distanceAB = stoi(std::string(_.at(0)));
				distances.push_back({ stopA, stopB, distanceAB });
			}

			return distances;
		}
	}//namespace distance
	namespace bus {
		Bus processing(const std::string_view record, TransportCatalogue& catalogue) {
			using namespace detail;

			Bus bus;
			auto rec = SplitString(record, MAIN_SPLIT);
			bus.name = cutWord(rec.at(0));

			if (rec.at(1).find(DELIMITER_ROUTE) != std::string_view::npos) {
				for (auto& stop : SplitString(rec.at(1), LINE_ROUTE)) {
					bus.stops.push_back(catalogue.getStop(cutWord(stop)));
				}
			}
			else {
				auto bus_route_ring = SplitString(rec.at(1), RING_ROUTE);
				auto& bus_route_line = bus_route_ring;

				//переводим кольцевоц маршрут в прямой
				for (int i = int(bus_route_ring.size() - 2); i > -1; --i) {
					bus_route_line.push_back(bus_route_ring.at(i));
				}

				for (auto& stop : bus_route_line) {
					bus.stops.push_back(catalogue.getStop(cutWord(stop)));
				}
			}
			return bus;
		}
	}//namespace processing
	namespace console_data_processing {
		void readData(std::istream& input, TransportCatalogue& catalogue) {
			int count = 0;
			input >> count;

			if (!count) return;

			std::vector<std::string> stops, buses, distances;

			std::string line;
			for (int i = 0; i != count; ++i) {

				getline(input >> std::ws, line);
				if (!line.size()) return;

				if (line[0] == stop::DELIMITER) {
					auto _ = line.substr(stop::BEGIN_LINE.size(), line.size());
					stops.push_back(_);
					distances.push_back(_);
				}
				else if (line[0] == bus::DELIMITER) {
					buses.push_back(line.substr(bus::BEGIN_LINE.size(), line.size()));
				}
				else {
					std::cout << "Input error" << std::endl;
					return;
				}
			}

			for (auto& stop : stops) {
				catalogue.addStop(stop::processing(stop));
			}

			for (auto& distance : distances) {
				catalogue.addDistance(distance::processing(distance, catalogue));
			}

			for (auto& bus : buses) {
				catalogue.addBus(bus::processing(bus, catalogue));
			}
		}
	} //namespace console_data_processing
}