#ifndef LOGGING_H
#define LOGGING_H

#include <string>

///// LOGGING LEVELS /////

// Levels as Numbers (in order of increasing severity)
#define INFO 0
#define WARN 1
#define ERROR 2

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

#ifdef ENABLE_LOGGING

// Basically, the logging level only allows logs equal
// to its level and above (in severity) to be printed
#if LOGGING_LEVEL == INFO
    #define LOG_INFO(msg) found::Log(INFO, msg)
    #define LOG_WARN(msg) found::Log(WARN, msg)
    #define LOG_ERROR(msg) found::Log(ERROR, msg)
#elif LOGGING_LEVEL == WARN
    #define LOG_INFO(msg)
    #define LOG_WARN(msg) found::Log(WARN, msg)
    #define LOG_ERROR(msg) found::Log(ERROR, msg)
#else
    #define LOG_INFO(msg)
    #define LOG_WARN(msg)
    #define LOG_ERROR(msg) found::Log(ERROR, msg)
#endif

namespace found {

///// INTERNAL Definitions /////

// The output streams to where the logs go. You must
// place your definitions of each within each ifndef
// statement
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
 * Logs a message
 * 
 * @param level The level
 * @param message The message to log
 */
void Log(int level, const std::string message);

}  // namespace found

#else

#define LOG_INFO(msg)
#define LOG_WARN(msg)
#define LOG_ERROR(msg)

#endif

#endif  // LOGGING_H
