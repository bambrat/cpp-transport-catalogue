#include "input_reader.h"

namespace transport_catalogue {
	//значение char c для CutWord()  
	const char CUT_DEFAULT = ' ';
	//значение splitter для SplitString() 
	const std::string SPLIT_DEFAULT = ", ";
	//значение для основного разделителя входных данных
	const std::string MAIN_SPLIT = ": ";

	const char BUS_DELIMITER_ROUTE = '>';
	const std::string BUS_LINE_ROUTE = ">";
	const std::string BUS_RING_ROUTE = "-";

	const std::string DISTANCE_DELIMITER = "m to ";
	//минимальный размер вектора после парсинга строки с информацией о дистанции
	const int MIN_VECTOR_SIZE = 3;

	namespace console_data_processing {
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

		Stop processingStop(const std::string_view record) {
			Stop stop;
			auto _ = SplitString(record, MAIN_SPLIT);
			stop.name = cutWord(_.at(0));

			auto coords = SplitString(_.at(1));
			stop.coord.lat = stod(std::string(coords.at(0)));
			stop.coord.lng = stod(std::string(coords.at(1)));
			return stop;
		}

		void processingDistance(const std::string_view record, TransportCatalogue& catalogue) {
			auto rec = SplitString(record);

			if (rec.size() < MIN_VECTOR_SIZE) return;

			auto stopA = catalogue.getStop(SplitString(record, MAIN_SPLIT).at(0));

			for (int i = 2; i < rec.size(); ++i) {
				auto _ = SplitString(rec.at(i), DISTANCE_DELIMITER);
				auto stopB = catalogue.getStop(_.at(1));
				auto distanceAB = stoi(std::string(_.at(0)));
				catalogue.addDistance(stopA, stopB, distanceAB);
			}
		}

		Bus processingBus(const std::string_view record) {
			Bus bus;
			auto rec = SplitString(record, MAIN_SPLIT);
			bus.name = cutWord(rec.at(0));

			if (rec.at(1).find(BUS_DELIMITER_ROUTE) != std::string_view::npos) {
				for (auto& stop : SplitString(rec.at(1), BUS_LINE_ROUTE)) {
					bus.stops_list.push_back((cutWord(stop)));
				}
			}
			else {
				auto bus_route_ring = SplitString(rec.at(1), BUS_RING_ROUTE);
				auto& bus_route_line = bus_route_ring;

				for (int i = int(bus_route_ring.size() - 2); i > -1; --i) {
					bus_route_line.push_back(bus_route_ring.at(i));
				}

				for (auto& stop : bus_route_line) {
					bus.stops_list.push_back((cutWord(stop)));
				}
			}
			return bus;
		}


		void readData(std::istream& input, TransportCatalogue& catalogue) {
			using namespace detail;
			int count = 0;
			input >> count;

			if (!count) return;

			std::vector<std::string> stops, buses, distances;

			std::string line;
			for (int i = 0; i != count; ++i) {

				getline(input >> std::ws, line);
				if (!line.size()) return;

				if (line[0] == STOP_DELIMITER) {
					auto _ = line.substr(STOP_BEGIN_LINE.size(), line.size());
					stops.push_back(_);
					distances.push_back(_);
				}
				else if (line[0] == BUS_DELIMITER) {
					buses.push_back(line.substr(BUS_BEGIN_LINE.size(), line.size()));
				}
				else {
					std::cout << "Input error" << std::endl;
					return;
				}
			}

			for (auto& stop : stops) {
				catalogue.addStop(processingStop(stop));
			}

			for (auto& distance : distances) {
				processingDistance(distance, catalogue);
			}

			for (auto& bus : buses) {
				catalogue.addBus(processingBus(bus));
			}
		}
	} //namespace console_data_processing
}