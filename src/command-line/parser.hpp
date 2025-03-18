#ifndef PARSER_H
#define PARSER_H

#include "command-line/options.hpp"

namespace found {

/**
 * This is where the program starts.
 * 
 * @param argc The number of arguments passed into the command line
 * @param argv The arguments passed into the command line
 * 
 * @return An integer indicating success (0) iff the program executes
 * successfully
 * 
 * @note The method itself uses command line arguments to generate
 * an Options object that represents all the algorithms we want to run
 * and their parameters
*/
int main(int argc, char **argv);

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


#endif  // PARSER_H
