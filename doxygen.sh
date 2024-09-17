#!/bin/bash

# Update the Doxygen file
doxygen -u > /dev/null 2>&1
rm -rf Doxyfile.bak

# Run Doxygen and capture output
warnings=$(doxygen 2>&1 1>/dev/tty)

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