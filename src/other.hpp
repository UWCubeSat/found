class Options {
public:
#define FOUND_CLI_OPTION(name, type, prop, defaultVal, converter, defaultArg) \
    type prop = defaultVal;
#include "options.hpp"
#undef FOUND_CLI_OPTION
};