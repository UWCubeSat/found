#!/usr/bin/env bash
set -euo pipefail

# Print and execute a command with a banner
execute_cmd() {
    echo ">>> $*"
    eval "$@"
}

# Utility: Run build in Docker container
run_in_container() {
    local BUILD_CMD="$1"

    mkdir -p logs
    TS=$(date +"%Y%m%d-%H%M%S")
    LOGFILE="logs/build-$TS.log"

    IMAGE_NAME="found-build-image"
    docker build --platform linux/amd64 -f .devcontainer/Dockerfile -t $IMAGE_NAME .

    docker run --rm \
        --platform linux/amd64 \
        --mount type=bind,src="$(pwd)",dst=/workspace \
        -w /workspace \
        $IMAGE_NAME \
        bash -c "$BUILD_CMD" &> "$LOGFILE"

    echo "Build logs saved to $LOGFILE"
    exit 0
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

# Check for --container flag remove it and update args if present
USE_CONTAINER=0
for arg in "$@"; do
    if [ "$arg" = "--container" ]; then
        USE_CONTAINER=1
        # Remove --container from args
        ARGS=()
        for arg in "$@"; do
            if [ "$arg" != "--container" ]; then
                ARGS+=("$arg")
            fi
        done
        set -- "${ARGS[@]}"
        break
    fi
done

# Parse the first argument to determine action
case "$1" in
    cmake)
        shift
        CONFIG_OPTS="${1:-}"  # Use empty string if not set
        if [ $# -gt 0 ]; then shift; fi

        CMD="mkdir -p build && cd build && cmake $CONFIG_OPTS .. && cmake --build . $*"
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

# Run command (use container if specified)
if [ "${USE_CONTAINER}" -eq 0 ]; then
    execute_cmd "$CMD"
else
    run_in_container "$CMD"
fi
