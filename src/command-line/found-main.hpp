#ifndef FOUND_MAIN_H
#define FOUND_MAIN_H

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

#endif  // FOUND_MAIN_H
