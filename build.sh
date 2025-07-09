#!/bin/bash

if [ "$1" = "cmake" ]; then
    mkdir -p build
    cd build
    echo cmake $2 ..
    cmake $2 .. 
    echo cmake --build . $3
    cmake --build . $3
elif [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    echo "Usage: ./build.sh make [GNU Make Options]"
    echo "Usage: ./build.sh cmake \"[CMake Config Options]\" \"[CMake Build Options]\""
elif [ "$1" = "clean" ]; then
    rm -rf build
elif [ "$1" = "clean_all" ]; then
    rm -rf build .cache
else
    shift
    make "$@"
fi
