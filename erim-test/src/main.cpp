// #include "serializer.hpp"
// #include "deserializer.hpp"
// #include "file_utils.hpp"
// #include <iostream>
// #include <cstring>

// class A {
// public:
//     A();
//     A(A& other);
//     ~A();
//     int q;
// };

// A::A() {
//     std::cout << "Constructor" << std::endl;
//     this->q = 23;
// }

// A::A(A& other) {
//     std::cout << "Copy Constructor" << std::endl;
//     this->q = other.q;
// }

// A::~A() {
//     std::cout << "Destructor " << this->q << std::endl;
// }


// std::string foo() {
//     std::string a = "fPP";
//     return a;
// }

// int main() {
//     std::cout << foo() << std::endl;
// //    delete a;
// }


// // void foo(int& a) {
// //     a = 7;
// // }

// // void main3() {
// //     int a = 42;
// //     std::cout << "A:" << a << std::endl;
// //     foo(a);
// //     std::cout << "A:" << a << std::endl;
// // }

// // void bar(int* a) {
// //     *a = 7;
// // }

// // int main() {
// //     int a = 42;
// //     std::cout << "A:" << a << std::endl;
// //     bar(&a);
// //     std::cout << "A:" << a << std::endl;
// // }


// int main2() {
//     // Create DataFile object
//     DataFile data;
//     std::strcpy(data.coord_system, "GLOBAL");
//     data.positions = { {1.0, 2.0, 3.0}, {4.5, 5.5, 6.5}, {7.0, 8.0, 9.0} };

//     // Define filename
//     std::string filename = "coordinates.found";

//     // Ensure correct file extension
//     if (!hasValidExtension(filename, ".found")) {
//         std::cerr << "Invalid file extension. Use .found" << std::endl;
//         return 1;
//     }

//     // Serialize Data
//     serialize(data, filename);
//     std::cout << "Data serialized to " << filename << std::endl;

//     // Deserialize Data
//     DataFile loadedData = deserialize(filename);

//     // Print deserialized data
//     std::cout << "Coordinate System: " << loadedData.coord_system << std::endl;
//     for (const auto& pos : loadedData.positions) {
//         std::cout << "Position: (" << pos.x << ", " << pos.y << ", " << pos.z << ")\n";
//     }

//     return 0;
// }



#include "serializer.hpp"
#include <iostream>
#include <cstring>

int main() {
    // Create a DataFile object
    DataFile data;
    std::strcpy(data.header.coord_system, "GLOBAL");
    data.header.num_positions = 3;
    data.positions = { {1.0, 2.0, 3.0}, {4.5, 5.5, 6.5}, {7.0, 8.0, 9.0} };

    // Define filename
    std::string filename = "data/coordinates.found";

    // Serialize the data
    serialize(data, filename);
    std::cout << "Data serialized to " << filename << std::endl;

    // Deserialize the data
    DataFile loadedData = deserialize(filename);
    std::cout << "Data deserialized from " << filename << std::endl;

    // Print deserialized data
    std::cout << "Coordinate System: " << loadedData.header.coord_system << std::endl;
    for (const auto& pos : loadedData.positions) {
        std::cout << "Position: (" << pos.x << ", " << pos.y << ", " << pos.z << ")\n";
    }

    // Read only the header
    DataFileHeader header = readHeader(filename);
    std::cout << "Header read: Coordinate System = " << header.coord_system
              << ", Num Positions = " << header.num_positions << std::endl;

    return 0;
}