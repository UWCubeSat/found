#!/usr/bin/env bash

# Print and execute a command with a banner
execute_cmd() {
    echo "$@"
    "$@"
}

# Display help message
display_help() {
    echo "Usage: ./build.sh make [GNU Make Options]"
    echo "Usage: ./build.sh cmake \"[CMake Config Options]\" [CMake Build Options]"
    echo "Usage: ./build.sh clean"
    echo "Usage: ./build.sh clean_all"
}

# Run the correct command based on the first argument
if [ "$1" = "cmake" ]; then
    shift
    mkdir -p build && cd build
    echo cmake $1 ..
    cmake $1 .. 
    shift
    CMD="cmake --build . $@"
elif [ "$1" = "make" ]; then
    shift
    CMD="make $@"
elif [ "$1" = "clean" ]; then
    CMD="rm -rf build"
elif [ "$1" = "clean_all" ]; then
    CMD="rm -rf build .cache"
elif [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    display_help
else
    display_help
fi

execute_cmd $CMD
