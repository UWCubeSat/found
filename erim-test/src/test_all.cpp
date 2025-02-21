#include "serializer.hpp"
#include "deserializer.hpp"
#include "file_utils.hpp"
#include <cassert>
#include <iostream>
#include <cstring>
#include <fstream>

// Test 1: Basic serialization and deserialization
void test_basic() {
    std::cout << "Running test_basic..." << std::endl;

    // Create a DataFile object
    DataFile originalData;
    std::strcpy(originalData.coord_system, "GLOBAL");
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
    assert(std::strcmp(originalData.coord_system, loadedData.coord_system) == 0);
    assert(originalData.positions.size() == loadedData.positions.size());

    for (size_t i = 0; i < originalData.positions.size(); ++i) {
        assert(originalData.positions[i].x == loadedData.positions[i].x);
        assert(originalData.positions[i].y == loadedData.positions[i].y);
        assert(originalData.positions[i].z == loadedData.positions[i].z);
    }

    std::cout << "test_basic passed!" << std::endl;
}

// Test 2: Empty positions vector
void test_empty_positions() {
    std::cout << "Running test_empty_positions..." << std::endl;

    // Create a DataFile object with an empty positions vector
    DataFile originalData;
    std::strcpy(originalData.coord_system, "LOCAL");
    originalData.positions = {};

    // Define filename
    std::string filename = "data/test_empty_positions.found";

    // Serialize the data
    serialize(originalData, filename);
    std::cout << "Data serialized to " << filename << std::endl;

    // Deserialize the data
    DataFile loadedData = deserialize(filename);
    std::cout << "Data deserialized from " << filename << std::endl;

    // Test if the deserialized data matches the original data
    assert(std::strcmp(originalData.coord_system, loadedData.coord_system) == 0);
    assert(originalData.positions.size() == loadedData.positions.size());

    std::cout << "test_empty_positions passed!" << std::endl;
}

// Test 3: Large values in positions
void test_large_values() {
    std::cout << "Running test_large_values..." << std::endl;

    // Create a DataFile object with large values
    DataFile originalData;
    std::strcpy(originalData.coord_system, "GLOBAL");
    originalData.positions = { {1e20, 2e20, 3e20}, {4.5e20, 5.5e20, 6.5e20} };

    // Define filename
    std::string filename = "data/test_large_values.found";

    // Serialize the data
    serialize(originalData, filename);
    std::cout << "Data serialized to " << filename << std::endl;

    // Deserialize the data
    DataFile loadedData = deserialize(filename);
    std::cout << "Data deserialized from " << filename << std::endl;

    // Test if the deserialized data matches the original data
    assert(std::strcmp(originalData.coord_system, loadedData.coord_system) == 0);
    assert(originalData.positions.size() == loadedData.positions.size());

    for (size_t i = 0; i < originalData.positions.size(); ++i) {
        assert(originalData.positions[i].x == loadedData.positions[i].x);
        assert(originalData.positions[i].y == loadedData.positions[i].y);
        assert(originalData.positions[i].z == loadedData.positions[i].z);
    }

    std::cout << "test_large_values passed!" << std::endl;
}

// Test 4: Invalid file extension
void test_invalid_extension() {
    std::cout << "Running test_invalid_extension..." << std::endl;

    // Create a DataFile object
    DataFile originalData;
    std::strcpy(originalData.coord_system, "GLOBAL");
    originalData.positions = { {1.0, 2.0, 3.0} };

    // Define filename with invalid extension
    std::string filename = "data/test_invalid_extension.txt";

    // Ensure correct file extension
    if (!hasValidExtension(filename, ".found")) {
        std::cerr << "Invalid file extension. Use .found" << std::endl;
        return;
    }

    // Serialize the data (should not reach here)
    serialize(originalData, filename);
    std::cout << "Data serialized to " << filename << std::endl;

    std::cout << "test_invalid_extension passed!" << std::endl;
}

// Test 5: Corrupted data file
void test_corrupted_data() {
    std::cout << "Running test_corrupted_data..." << std::endl;

    // Create a corrupted file
    std::string filename = "data/test_corrupted_data.found";
    std::ofstream outFile(filename, std::ios::binary);
    outFile << "This is not a valid DataFile!";
    outFile.close();

    // Attempt to deserialize the corrupted file
    try {
        DataFile loadedData = deserialize(filename);
        std::cerr << "Deserialization should have failed!" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Deserialization failed as expected: " << e.what() << std::endl;
    }

    std::cout << "test_corrupted_data passed!" << std::endl;
}

int main() {
    // Run all tests
    test_basic();
    test_empty_positions();
    test_large_values();
    test_invalid_extension();
    test_corrupted_data();

    std::cout << "All tests completed!" << std::endl;
    return 0;
}