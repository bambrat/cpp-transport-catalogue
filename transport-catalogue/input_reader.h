#pragma once
#include <sstream>

#include "transport_catalogue.h"

namespace transport_catalogue {
	namespace detail {
		//значение char c для CutWord()  
		const char CUT_DEFAULT = ' ';
		//значение splitter для SplitString() 
		const std::string SPLIT_DEFAULT = ", ";
		//значение для основного разделителя входных данных
		const std::string MAIN_SPLIT = ": ";
	}

	namespace stop {
		const char DELIMITER = 'S';
		const std::string BEGIN_LINE = "Stop ";
		Stop processing(const std::string_view record);
	}
	namespace bus {
		const char DELIMITER = 'B';
		const std::string BEGIN_LINE = "Bus ";
		const char DELIMITER_ROUTE = '>';
		const std::string LINE_ROUTE = ">";
		const std::string RING_ROUTE = "-";
		Bus processing(const std::string_view record, TransportCatalogue& catalogue);
	}
	namespace distance {
		const std::string DELIMITER = "m to ";
		//минимальный размер вектора после парсинга строки с информацией о дистанции
		const int MIN_VECTOR_SIZE = 3;
		std::vector <Distance > processing(const std::string_view record, TransportCatalogue& catalogue);
	}
	namespace console_data_processing {
		void readData(std::istream& input, transport_catalogue::TransportCatalogue& catalogue);
	}
}