#include "logging/logging.hpp"

#ifdef ENABLE_LOGGING

#include <iomanip>
#include <iostream>
#include <string>

namespace found {

void Log(int level, const std::string message) {
    // Determine Logging Level
    const std::string level_string = GET_LEVEL(level);

    // Determine UTC Time
    std::time_t now = std::time(nullptr);

    // Convert to local time
    std::tm *local_time = std::localtime(&now);

    // Print out everything
    GET_STREAM(level) <<
        "[" << level_string << " " << std::put_time(local_time, "%Y-%m-%d %H:%M:%S %Z") << "] " <<
        message << std::endl;
}

}  // namespace found

#endif
