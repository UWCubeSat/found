#ifndef OPTIONS_H
#define OPTIONS_H

// this file uses the "X" pattern.

// Arguments to FOUND_CLI_OPTION:
// 1. String used as the command line option.
// 2. Type of the option value.
// 3. Property name
// 4. Default value
// 5. Code to convert optarg into the value.
// 6. The default value if the flag is specified
// 7. The macro to use to assign a value to the variable

// To properly align these fields, I recommend using an editor plugin. In Vim, try `vim-lion`; in
// Emacs, try `evil-lion`. With your cursor inside any of the blocks, type `glip,` to aLign the
// Inside of the current Paragraph to comma.

#include <string>

#include "common/style.hpp"
#include "common/spatial/attitude-utils.hpp"
#include "providers/converters.hpp"

// TODO(nguy8tri): Change std::string values to proper values (i.e. output/input files should become streams, etc.)

// NOLINTBEGIN

/// Calibration Flags
#define CALIBRATION \
FOUND_CLI_OPTION("local-orientation"    , found::EulerAngles, lclOrientation    , found::EulerAngles(0, 0, 0), found::strtoea(optarg)  , kNoDefaultArgument, REQ_ASSIGN) \
FOUND_CLI_OPTION("reference-orientation", found::EulerAngles, refOrientation    , found::EulerAngles(0, 0, 0), found::strtoea(optarg)  , kNoDefaultArgument, REQ_ASSIGN) \
FOUND_CLI_OPTION("use-same-orientation" , bool              , useSameOrientation, false                      , found::strtobool(optarg), true              , OPT_ASSIGN) \
FOUND_CLI_OPTION("output-file"          , std::string       , outputFile        , ""                         , optarg                  , kNoDefaultArgument, REQ_ASSIGN)


/// Distance Flags
#define DISTANCE \
FOUND_CLI_OPTION("image"                   , found::Image      , image           , {}                         , found::strtoimage(optarg)  , kNoDefaultArgument, REQ_ASSIGN) \
FOUND_CLI_OPTION("calibration-data"        , std::string       , calibrationData , ""                         , optarg                     , kNoDefaultArgument, REQ_ASSIGN) \
FOUND_CLI_OPTION("reference-as-orientation", bool              , refAsOrientation, false                      , found::strtobool(optarg)   , true              , OPT_ASSIGN) \
FOUND_CLI_OPTION("camera-focal-length"     , decimal           , focalLength     , 0.012                      , found::strtodecimal(optarg), kNoDefaultArgument, REQ_ASSIGN) \
FOUND_CLI_OPTION("camera-pixel-size"       , decimal           , pixelSize       , 20E-6                      , found::strtodecimal(optarg), kNoDefaultArgument, REQ_ASSIGN) \
FOUND_CLI_OPTION("reference-orientation"   , found::EulerAngles, refOrientation  , found::EulerAngles(0, 0, 0), found::strtoea(optarg)     , kNoDefaultArgument, REQ_ASSIGN) \
FOUND_CLI_OPTION("relative-orientation"    , found::EulerAngles, relOrientation  , found::EulerAngles(0, 0, 0), found::strtoea(optarg)     , kNoDefaultArgument, REQ_ASSIGN)


// Orbit Flags
// TODO: Fix these all to correct parameters/outputs
#define ORBIT \
FOUND_CLI_OPTION("position-data", std::string, positionData, "", optarg, kNoDefaultArgument, REQ_ASSIGN) \
FOUND_CLI_OPTION("output-form"  , std::string, output      , "", optarg, kNoDefaultArgument, REQ_ASSIGN)

// NOLINTEND

class CalibrationOptions {
 public:
#define FOUND_CLI_OPTION(name, type, prop, defaultVal, converter, defaultArg, ASSIGN) \
    type prop = defaultVal;
    CALIBRATION
#undef FOUND_CLI_OPTION
};

class DistanceOptions {
 public:
#define FOUND_CLI_OPTION(name, type, prop, defaultVal, converter, defaultArg, ASSIGN) \
    type prop = defaultVal;
    DISTANCE
#undef FOUND_CLI_OPTION
};

class OrbitOptions {
 public:
#define FOUND_CLI_OPTION(name, type, prop, defaultVal, converter, defaultArg, ASSIGN) \
    type prop = defaultVal;
    ORBIT
#undef FOUND_CLI_OPTION
};

#endif  // OPTIONS_H
