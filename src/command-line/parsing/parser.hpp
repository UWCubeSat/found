#ifndef SRC_COMMAND_LINE_PARSING_PARSER_HPP_
#define SRC_COMMAND_LINE_PARSING_PARSER_HPP_

#include "command-line/parsing/options.hpp"

namespace found {

/// Defines the help message
#define HELP_MSG "Use ./found --help or ./found -h for help"

/**
 * Parses the calibration options from the command line to
 * run the calibration algorithm
 * 
 * @param argc The number of command-line arguments
 * @param argv The command line arguments
 * 
 * @return CalibrationOptions The options for the calibration
 * algorithm as extracted on the command line
 */
CalibrationOptions ParseCalibrationOptions(int argc, char **argv);

/**
 * Parses the distance options from the command line to
 * run the distance determination algorithm
 * 
 * @param argc The number of command-line arguments
 * @param argv The command line arguments
 * 
 * @return DistanceOptions The options for the distance determination
 * algorithm as extracted on the command line
 */
DistanceOptions ParseDistanceOptions(int argc, char **argv);

/**
 * Parses the orbit options from the command line to
 * run the orbit determination algorithm
 * 
 * @param argc The number of command-line arguments
 * @param argv The command line arguments
 * 
 * @return OrbitOptions The options for the orbit determination
 * algorithm as extracted from the command line
 */
OrbitOptions ParseOrbitOptions(int argc, char **argv);

}  // namespace found


#endif  // SRC_COMMAND_LINE_PARSING_PARSER_HPP_
