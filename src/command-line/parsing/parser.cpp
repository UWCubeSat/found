#include "command-line/parsing/parser.hpp"

#include <getopt.h>

#include <iostream>
#include <string>

#include "common/logging/logging.hpp"
#include "common/style.hpp"
#include "command-line/parsing/options.hpp"

/// This defines the global variable optind, used
/// for keeping track of parsing for command-line
/// arguments
int optind = 2;

/**
 * Determines whether the current flag allows an optional argument AND
 * if that argument exists
 */
#define OPTIONAL_OPTARG()                                        \
    ((optarg == NULL && optind < argc && argv[optind][0] != '-') \
    ? static_cast<bool>(optarg = argv[optind++])                 \
    : (optarg != NULL))

/**
 * Assigns a flag with a required value
 * 
 * @param options The options object that has the flag
 * @param prop The name of the prop within options
 * @param value The value to assign
 * @param default Not used
 */
#define REQ_ASSIGN(options, prop, value, default) \
    options.prop = (value);

/**
 * Assigns a flag with an optional value
 * 
 * @param options The options object that has the flag
 * @param prop The name of the prop within options
 * @param value The value to assign
 * @param default The default value
 * 
 * @pre Must be used with FOUND_CLI_OPTION
 */
#define OPT_ASSIGN(options, prop, value, default) \
    if (OPTIONAL_OPTARG()) {                      \
        options.prop = value;                     \
    } else {                                      \
        options.prop = default;                   \
    }

namespace found {

/// For macro processing
const char kNoDefaultArgument = 0;

int main(int argc, char **argv) {
    if (argc == 1) {
        LOG_INFO("Seems you don't want to be found");
        return EXIT_SUCCESS;
    }
    std::string command(argv[1]);

    // TODO(skyler): Implement Logic to figure out what algorithm we need,
    // and call appropriate Generate* functions

    return EXIT_SUCCESS;
}

CalibrationOptions ParseCalibrationOptions(int argc, char **argv) {
    // Define an enum for each valid flag (command-line entry), which maps it from the name
    // to an integer
    enum class ClientOption {
        #define FOUND_CLI_OPTION(name, type, prop, defaultVal, converter, defaultArg, ASSIGN) \
                prop,
        CALIBRATION
        #undef FOUND_CLI_OPTION
    };

    // Define an array of options, which defines the traits pertaining to each
    // expected command-line entry
    static option long_options[] = {
        #define FOUND_CLI_OPTION(name, type, prop, defaultVal, converter, defaultArg, ASSIGN) \
                {name,                                                                        \
                defaultArg == kNoDefaultArgument ? required_argument : optional_argument,     \
                0,                                                                            \
                static_cast<int>(ClientOption::prop)},
        CALIBRATION
        #undef FOUND_CLI_OPTION
        {0}
    };

    // Define our result, and iterator helpers
    CalibrationOptions options;
    int index;
    int option;

    // Iterates through the list of command-line tokens and figures out
    // what data to assign to which field in options. Note that the
    // FOUND_CLI_OPTION defines the conversion already between any
    // particular parameter (as a string) to its actual type
    while ((option = getopt_long(argc, argv, "", long_options, &index)) != -1) {
        switch (option) {
            #define FOUND_CLI_OPTION(name, type, prop, defaultVal, converter, defaultArg, ASSIGN) \
                    case static_cast<int>(ClientOption::prop):                                    \
                        ASSIGN(options, prop, converter, defaultArg)                              \
                        break;
            CALIBRATION
            #undef FOUND_CLI_OPTION
            default:
                LOG_ERROR("Illegal flag detected");
                exit(EXIT_FAILURE);
                break;
        }
    }

    return options;
}

// TODO(skyler): Implement Generate* functions here (example is above for you)

}  // namespace found
