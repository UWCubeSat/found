#include "serializer.hpp"
#include "deserializer.hpp"
#include "file_utils.hpp"
#include <iostream>
#include <cstring>

void foo(int* a) {
    a++;
    a++;
    *a = 99;
}


// int main2() {
//     long a = 7;
//     long* b = &a;
//     long* c = b;
//     std::cout << "A1:" << a << std::endl;
//     std::cout << "B1:" << b << std::endl;
//     std::cout << "B1:" << *b << std::endl;
//     std::cout << "C1:" << c << std::endl;
//     std::cout << "C1:" << *c << std::endl << std::endl;

//     *c = 19;
//     std::cout << "A1:" << a << std::endl;
//     std::cout << "B1:" << b << std::endl;
//     std::cout << "B1:" << *b << std::endl;
//     std::cout << "C1:" << c << std::endl;
//     std::cout << "C1:" << *c << std::endl;
// }


int main() {
    // Create DataFile object
    DataFile data;
    std::strcpy(data.coord_system, "GLOBAL");
    data.positions = { {1.0, 2.0, 3.0}, {4.5, 5.5, 6.5}, {7.0, 8.0, 9.0} };

    // Define filename
    std::string filename = "coordinates.found";

    // Ensure correct file extension
    if (!hasValidExtension(filename, ".found")) {
        std::cerr << "Invalid file extension. Use .found" << std::endl;
        return 1;
    }

    // Serialize Data
    serialize(data, filename);
    std::cout << "Data serialized to " << filename << std::endl;

    // Deserialize Data
    DataFile loadedData = deserialize(filename);

    // Print deserialized data
    std::cout << "Coordinate System: " << loadedData.coord_system << std::endl;
    for (const auto& pos : loadedData.positions) {
        std::cout << "Position: (" << pos.x << ", " << pos.y << ", " << pos.z << ")\n";
    }

    return 0;
}