#!/bin/bash

if [ "$1" = "cmake" ]; then
    shift
    mkdir -p build && cd build
    echo cmake $1 ..
    cmake $1 .. 
    shift
    echo cmake --build . $@
    cmake --build . $@
elif [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    echo "Usage: ./build.sh make [GNU Make Options]"
    echo "Usage: ./build.sh cmake \"[CMake Config Options]\" [CMake Build Options]"
    echo "Usage: ./build.sh clean"
    echo "Usage: ./build.sh clean_all"
elif [ "$1" = "clean" ]; then
    echo "rm -rf build"
    rm -rf build
elif [ "$1" = "clean_all" ]; then
    echo "rm -rf build .cache"
    rm -rf build .cache
elif [ "$1" = "make" ]; then
    shift
<<<<<<< HEAD
    echo make --build . "$@"
    make "$@"
=======
    make $@
>>>>>>> main
else
    echo "Usage: ./build.sh make [GNU Make Options]"
    echo "Usage: ./build.sh cmake \"[CMake Config Options]\" [CMake Build Options]"
    echo "Usage: ./build.sh clean"
    echo "Usage: ./build.sh clean_all"
fi
