class Options {
 public:
#define FOUND_CLI_OPTION(name, type, prop, defaultVal, converter, defaultArg) \
    type prop = defaultVal;
#include "command-line/options.hpp"
#undef FOUND_CLI_OPTION
};
