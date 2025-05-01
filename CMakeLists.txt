cmake_minimum_required(VERSION 3.10)
project(FOUND)
set(CMAKE_CXX_STANDARD 11)
include_directories(include)
add_library(edge_detection
    src/edge_detection/simple-edge-detection.cpp
)
add_executable(test_edge_detection
    test/edge_detection/test-simple-edge-detection.cpp
)
target_link_libraries(test_edge_detection edge_detection)
