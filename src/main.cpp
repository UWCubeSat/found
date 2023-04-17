
#include <getopt.h>

#include <iostream>
#include <string>

#include "other.hpp"
#include "style.hpp"

namespace found {

// For macro processing
const char kNoDefaultArgument = 0;

// For command-line processing
#define LOST_OPTIONAL_OPTARG()                                   \
    ((optarg == NULL && optind < argc && argv[optind][0] != '-') \
     ? (bool) (optarg = argv[optind++])                          \
     : (optarg != NULL))


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
int FoundMain(int argc, char **argv) {
    if(argc == 1) {
        std::cout << "Seems you don't want to be found";
        return 0;
    }
    std::string command(argv[1]);
    int optind = 2;

    enum class DatabaseCliOption {
        #define FOUND_CLI_OPTION(name, type, prop, defaultVal, converter, defaultArg) prop,
        #include "options.hpp"
        #undef FOUND_CLI_OPTION
    };

    static struct option long_options[] = {
        #define FOUND_CLI_OPTION(name, type, prop, defaultVal, converter, defaultArg) \
                    {name,                                                            \
                    defaultArg == 0 ? required_argument : optional_argument,          \
                    0,                                                                \
                    (int)DatabaseCliOption::prop},
        #include "options.hpp" // NOLINT
        #undef FOUND_CLI_OPTION
                        {0}
    };

    Options options;
        int index;
        int option;

        while ((option = getopt_long(argc, argv, "", long_options, &index)) != -1) {
            switch (option) {
#define FOUND_CLI_OPTION(name, type, prop, defaultVal, converter, defaultArg) \
                case (int)DatabaseCliOption::prop :                           \
                    if (defaultArg == 0) {                                    \
                        options.prop = converter;                             \
                    } else {                                                  \
                        if (LOST_OPTIONAL_OPTARG()) {                         \
                            options.prop = converter;                         \
                        } else {                                              \
                            options.prop = defaultArg;                        \
                        }                                                     \
                    }                                                         \
            break;
#include "options.hpp" // NOLINT
#undef FOUND_CLI_OPTION
                default :std::cout << "Illegal flag" << std::endl;
                    exit(1);
            }
        }
    return 0;

}

}

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
    found::FoundMain(argc, argv);
    return 0;
}