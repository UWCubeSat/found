#ifndef SRC_COMMAND_LINE_FOUND_MAIN_HPP_
#define SRC_COMMAND_LINE_FOUND_MAIN_HPP_

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

}  // namespace found

#endif  // SRC_COMMAND_LINE_FOUND_MAIN_HPP_
