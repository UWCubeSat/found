#!/bin/bash

# Exit on any command failure
set -e

# Update the Doxygen file
doxygen -u > /dev/null 2>&1
rm -rf Doxyfile.bak

# Run Doxygen and capture stderr while preserving stdout
doxygen 2> stderr.log

# Extract warnings from stderr file
warnings=$(grep "warning" stderr.log)

# Cleanup the stderr file
rm stderr.log

# Check for warnings/errors in the log file
if [ -n "$warnings" ]; then
    printf "\nDoxygen found errors and warnings while generating documentation:\n"
    echo $warnings
    printf "\n"
    exit 1
else
    printf "\nDoxygen completed successfully with no warnings.\n"
    exit 0
fi