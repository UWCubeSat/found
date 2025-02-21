#include "serializer.hpp"
#include "deserializer.hpp"
#include "data_structures.hpp"
#include <iostream>
#include <cassert>
#include <cstring>


void test_basic() {
    std::cout << "Running test_basic..." << std::endl;

    // Create a DataFile object
    DataFile originalData;
    std::strcpy(originalData.header.coord_system, "GLOBAL");
    originalData.header.num_positions = 3;
    originalData.positions = { {1.0, 2.0, 3.0}, {4.5, 5.5, 6.5}, {7.0, 8.0, 9.0} };

    // Define filename
    std::string filename = "data/test_basic.found";

    // Serialize the data
    serialize(originalData, filename);
    std::cout << "Data serialized to " << filename << std::endl;

    // Deserialize the data
    DataFile loadedData = deserialize(filename);
    std::cout << "Data deserialized from " << filename << std::endl;

    // Test if the deserialized data matches the original data
    assert(std::strcmp(originalData.header.coord_system, loadedData.header.coord_system) == 0);
    assert(originalData.positions.size() == loadedData.positions.size());

    for (size_t i = 0; i < originalData.positions.size(); ++i) {
        assert(originalData.positions[i].x == loadedData.positions[i].x);
        assert(originalData.positions[i].y == loadedData.positions[i].y);
        assert(originalData.positions[i].z == loadedData.positions[i].z);
    }

    std::cout << "test_basic passed!" << std::endl;
}

int main() {
    test_basic();
    return 0;
}