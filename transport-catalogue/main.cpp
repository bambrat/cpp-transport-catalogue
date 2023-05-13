#include "stat_reader.h"
#include "input_reader.h"
#include "transport_catalogue.h"

using namespace transport_catalogue;

int main() {
    TransportCatalogue catalogue;
    
    console_data_processing::readData(std::cin, catalogue);
    console_data_processing::readRequest(std::cin, catalogue);
    return 0;
}