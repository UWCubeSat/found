#!/bin/bash

# Update the Doxygen file
doxygen -u > /dev/null 2>&1
rm -rf Doxyfile.bak
mkdir -p .cache

LOG_FILE=".cache/stderr.log"

# Run Doxygen and capture stderr while preserving stdout
doxygen 2> $LOG_FILE

# Extract warnings from stderr file
warnings=$(cat $LOG_FILE)

# Cleanup the stderr file
rm $LOG_FILE

# Check for warnings/errors in the log file
if [ -n "$warnings" ]; then
    printf "\nDoxygen found errors and warnings while generating documentation:\n"
    echo "$warnings"
    printf "\n"
    exit 1
else
    printf "\nDoxygen completed successfully with no warnings.\n"
    exit 0
fi