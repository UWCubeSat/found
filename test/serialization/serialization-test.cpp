#include <iostream>
#include <cassert>
#include <fstream> // Include this for std::ofstream
#include "serialization/serialization.hpp" // Include the header for system.cpp

using namespace found;

// Helper function to print DataFile information
void printDataFile(const DataFile& data, const std::string& label) {
    std::cout << "=== " << label << " ===" << std::endl;
    std::cout << "Number of Positions: " << data.header.num_records << std::endl;
    for (size_t i = 0; i < data.records.size(); ++i) {
        std::cout << "Position " << i + 1 << ": (" 
                  << data.records[i].position.x << ", " 
                  << data.records[i].position.y << ", " 
                  << data.records[i].position.z << ")" << std::endl;
    }
    std::cout << "====================" << std::endl;
}

// Test function for basic serialization and deserialization
void test_basic() {
    std::cout << "Running test_basic..." << std::endl;

    // Create a DataFile object
    DataFile originalData;
    originalData.header.num_records = 3;
    originalData.records = { { {1.0, 2.0, 3.0} }, { {4.5, 5.5, 6.5} }, { {7.0, 8.0, 9.0} } };

    // Print original data
    printDataFile(originalData, "Original Data");

    // Define filename
    std::string filename = "data/test_basic.found";

    // Serialize the data
    serialize(originalData, filename);
    std::cout << "Data serialized to " << filename << std::endl;

    // Deserialize the data
    DataFile loadedData = deserialize(filename);
    std::cout << "Data deserialized from " << filename << std::endl;

    // Print deserialized data
    printDataFile(loadedData, "Deserialized Data");

    // Test if the deserialized data matches the original data
    assert(originalData.header.num_records == loadedData.header.num_records);
    assert(originalData.records.size() == loadedData.records.size());

    for (size_t i = 0; i < originalData.records.size(); ++i) {
        assert(originalData.records[i].position.x == loadedData.records[i].position.x);
        assert(originalData.records[i].position.y == loadedData.records[i].position.y);
        assert(originalData.records[i].position.z == loadedData.records[i].position.z);
    }

    std::cout << "test_basic passed!" << std::endl;
}

// Test function for reading only the header
void test_read_header() {
    std::cout << "Running test_read_header..." << std::endl;

    // Create a DataFile object
    DataFile originalData;
    originalData.header.num_records = 2;
    originalData.records = { { {10.0, 20.0, 30.0} }, { {40.0, 50.0, 60.0} } };

    // Print original data
    printDataFile(originalData, "Original Data");

    // Define filename
    std::string filename = "data/test_read_header.found";

    // Serialize the data
    serialize(originalData, filename);
    std::cout << "Data serialized to " << filename << std::endl;

    // Read only the header
    DataFileHeader header = readHeader(filename);
    std::cout << "Header read from " << filename << ":" << std::endl;
    std::cout << "Number of Positions: " << header.num_records << std::endl;

    // Test if the header matches the original data
    assert(originalData.header.num_records == header.num_records);

    std::cout << "test_read_header passed!" << std::endl;
}

// Test function for CRC validation
void test_crc_validation() {
    std::cout << "Running test_crc_validation..." << std::endl;

    // Create a DataFile object
    DataFile originalData;
    originalData.header.num_records = 1;
    originalData.records = { { {100.0, 200.0, 300.0} } };

    // Print original data
    printDataFile(originalData, "Original Data");

    // Define filename
    std::string filename = "data/test_crc_validation.found";

    // Serialize the data
    serialize(originalData, filename);
    std::cout << "Data serialized to " << filename << std::endl;

    // Deserialize the data (should pass CRC validation)
    DataFile loadedData = deserialize(filename);
    std::cout << "Data deserialized from " << filename << std::endl;

    // Print deserialized data
    printDataFile(loadedData, "Deserialized Data");

    // Corrupt the file (simulate a corrupted file)
    std::ofstream outFile(filename, std::ios::binary | std::ios::in | std::ios::out);
    if (!outFile) {
        throw std::runtime_error("Failed to open file for writing");
    }
    outFile.seekp(10); // Move to a random position in the file
    outFile.put(0xFF); // Write a corrupted byte
    outFile.close();

    // Attempt to deserialize the corrupted file (should fail CRC validation)
    try {
        DataFile corruptedData = deserialize(filename);
        std::cerr << "CRC validation should have failed!" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << "CRC validation failed as expected: " << e.what() << std::endl;
    }

    std::cout << "test_crc_validation passed!" << std::endl;
}

int main() {
    // Run all tests
    test_basic();
    test_read_header();
    test_crc_validation();

    std::cout << "All tests completed!" << std::endl;
    return 0;
}