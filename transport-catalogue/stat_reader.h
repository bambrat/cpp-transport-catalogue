#pragma once
#include "input_reader.h"

namespace transport_catalogue {
	namespace console_data_processing {
		void readRequest(std::istream& input, const TransportCatalogue& catalogue);
	}
}