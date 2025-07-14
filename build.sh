#!/usr/bin/env bash
set -euo pipefail

# Print and execute a command with a banner
execute_cmd() {
    echo ">>> $*"
    eval "$@"
}

# Display help message
display_help() {
    echo "Usage:"
    echo "  ./build.sh cmake \"[CMake Config Options]\" [CMake Build Options]"
    echo "  ./build.sh make [GNU Make Options]"
    echo "  ./build.sh clean"
    echo "  ./build.sh clean_all"
    echo "  ./build.sh --help | -h"
}

# Exit if no arguments were provided
if [ $# -eq 0 ]; then
    display_help
    exit 1
fi

# Parse first argument
case "$1" in
    cmake)
        shift
        mkdir -p build && cd build

        CONFIG_OPTS="${1:-}"  # Use empty string if not set
        if [ $# -gt 0 ]; then shift; fi

        CMD="cmake $CONFIG_OPTS .. && cmake --build . $*"
        ;;

    make)
        shift
        CMD="make $*"
        ;;

    clean)
        CMD="rm -rf build"
        ;;

    clean_all)
        CMD="rm -rf build .cache"
        ;;

    -h|--help)
        display_help
        exit 0
        ;;

    *)
        display_help
        exit 1
        ;;
esac

# Run the final command
execute_cmd "$CMD"
