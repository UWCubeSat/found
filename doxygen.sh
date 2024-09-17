#!/bin/bash

# Define paths
DOXYGEN_CONFIG="Doxyfile"
DOXYGEN_LOG_DIR="build/logs/doxygen"
DOXYGEN_LOG="$DOXYGEN_LOG_DIR/doxygen.log"

# Create the logs
mkdir -p $DOXYGEN_LOG_DIR

# Update the Doxygen file
doxygen -u > /dev/null 2>&1
rm -rf Doxyfile.bak

# Run Doxygen and capture output
warnings=$(doxygen "$DOXYGEN_CONFIG" 2>&1 1>/dev/null)

# Check for warnings/errors in the log file
if [ -n "$warnings" ]; then
    printf "\nDoxygen found errors and warnings while generating documentation:"
    echo $warnings
    printf "\n"
    exit 1
else
    printf "\nDoxygen completed successfully with no warnings.\n"
    exit 0
fi