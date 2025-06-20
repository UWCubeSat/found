#ifndef LOGGING_H
#define LOGGING_H

///// LOGGING LEVELS /////

// Levels as Numbers (in order of increasing severity)
#define INFO 0
#define WARN 1
#define ERROR 2

#ifdef ENABLE_LOGGING

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>

// Levels as Strings (in order of increasing severity)
#define INFO_STR "INFO"
#define WARN_STR "WARN"
#define ERROR_STR "ERROR"

/**
 * Obtains the correct level string given the level number
 * 
 * @param level (int) The level to get the string of
 */
#define GET_LEVEL(level) \
    (level == ERROR ? ERROR_STR : (level == WARN ? WARN_STR : INFO_STR))

///// LOGGING FUNCTIONS /////

// LOG_INFO, LOG_WARN and LOG_ERROR
// should be used within the program
// (see command-line/parser.cpp for examples)

// Basically, the logging level only allows logs equal
// to its level and above (in severity) to be printed
#if LOGGING_LEVEL == INFO
    #define LOG_INFO(msg) LOG(INFO, msg)
    #define LOG_WARN(msg) LOG(WARN, msg)
    #define LOG_ERROR(msg) LOG(ERROR, msg)
#elif LOGGING_LEVEL == WARN
    #define LOG_INFO(msg)
    #define LOG_WARN(msg) LOG(WARN, msg)
    #define LOG_ERROR(msg) LOG(ERROR, msg)
#else
    #define LOG_INFO(msg)
    #define LOG_WARN(msg)
    #define LOG_ERROR(msg) LOG(ERROR, msg)
#endif

namespace found {

///// INTERNAL Definitions /////

// The output streams to where the logs go. To override
// the default streams as shown below, you must define
// them yourselve above. For instance:
//
// // Define the streams as external variable in this file
// extern std::ofstream log_file;
// extern std::ofstream err_file;
//
// // Then, in logging.cpp (does not exist right now)
// // Define extern log files, opening in append mode
// std::ofstream log_file("path/to/logs/found.log", std::ios::app);
// std::ofstream err_file("path/to/logs/error.log", std::ios::app);
//
// #define INFO_STREAM log_file
// #define WARN_STREAM log_file
// #define ERROR_STREAM err_file
//
#ifndef INFO_STREAM
#define INFO_STREAM std::cout
#endif
#ifndef WARN_STREAM
#define WARN_STREAM std::cerr
#endif
#ifndef ERROR_STREAM
#define ERROR_STREAM std::cerr
#endif

/**
 * Determines the correct stream to use for logging
 * 
 * @param level (int) The logging level to use
 */
#define GET_STREAM(level) \
    (level == ERROR ? ERROR_STREAM : (level == WARN ? WARN_STREAM : INFO_STREAM))

/**
 * Logs a message to a particular level
 * 
 * @param level The level to log at
 * @param message The message to log
 */
#define LOG(level, message) {                                                                         \
    /* Determine Logging Level */                                                                     \
    const std::string level_string = GET_LEVEL(level);                                                \
    /* Determine UTC Time */                                                                          \
    std::time_t now = std::time(nullptr);                                                             \
    /* Convert to local time */                                                                       \
    std::tm *local_time = std::localtime(&now);                                                       \
    /* Print out everything */                                                                        \
    GET_STREAM(level) <<                                                                              \
        "[" << level_string << " " << std::put_time(local_time, "%Y-%m-%d %H:%M:%S %Z") << "] " <<    \
        message << std::endl;                                                                         \
}

}  // namespace found

#else

#define LOG_INFO(msg)
#define LOG_WARN(msg)
#define LOG_ERROR(msg)

#endif

#endif  // LOGGING_H
