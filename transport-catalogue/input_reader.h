#pragma once
#include "transport_catalogue.h"

namespace transport_catalogue {
	namespace console_data_processing {
		void readData(std::istream& input, transport_catalogue::TransportCatalogue& catalogue);
	}
	namespace detail {
		const char BUS_DELIMITER = 'B';
		const std::string BUS_BEGIN_LINE = "Bus ";
		const char STOP_DELIMITER = 'S';
		const std::string STOP_BEGIN_LINE = "Stop ";
	}
}