#include "command-line/parsing/parser.hpp"

/**
 * This is where the program starts.
 * 
 * @param argc The number of arguments passed into the command line
 * @param argv The arguments passed into the command line
 * 
 * @return An integer indicating success (0) iff the program executes
 * successfully
 * 
*/
int main(int argc, char **argv) {
    return found::main(argc, argv);
}
