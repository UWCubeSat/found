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
// 8. Documentation

// To properly align these fields, I recommend using an editor plugin. In Vim, try `vim-lion`; in
// Emacs, try `evil-lion`. With your cursor inside any of the blocks, type `glip,` to aLign the
// Inside of the current Paragraph to comma.

#include <string>

#include "common/style.hpp"
#include "common/spatial/attitude-utils.hpp"
#include "providers/converters.hpp"
#include "datafile/datafile.hpp"
#include "datafile/serialization.hpp"

#define emptyDFVer 0U
#define defaultDFHead {{'L','O','S','T'}, emptyDFVer}

// NOLINTBEGIN

/// Calibration Flags
#define CALIBRATION \
FOUND_CLI_OPTION("local-orientation"    , found::EulerAngles, lclOrientation    , found::EulerAngles(0, 0, 0), found::strtoea(optarg)  , kNoDefaultArgument, REQ_ASSIGN, "The local orientation (deg)"    ) \
FOUND_CLI_OPTION("reference-orientation", found::EulerAngles, refOrientation    , found::EulerAngles(0, 0, 0), found::strtoea(optarg)  , kNoDefaultArgument, REQ_ASSIGN, "The reference orientation (deg)") \
FOUND_CLI_OPTION("output-file"          , std::string       , outputFile        , ""                         , optarg                  , kNoDefaultArgument, REQ_ASSIGN, "The output file (.found)")


/// Distance Flags
#define DISTANCE \
FOUND_CLI_OPTION("image"                   , found::Image      , image           , {}                         , found::strtoimage(optarg)  , kNoDefaultArgument, REQ_ASSIGN, "The image to process"                                      ) \
FOUND_CLI_OPTION("calibration-data"        , found::DataFile   , calibrationData , {defaultDFHead}         , found::strtodf(optarg)     , kNoDefaultArgument, REQ_ASSIGN, "The calibration data (.found)"                             ) \
FOUND_CLI_OPTION("reference-as-orientation", bool              , refAsOrientation, false                      , found::strtobool(optarg)   , true              , OPT_ASSIGN, "Use reference-orientation as the orientation of the camera") \
FOUND_CLI_OPTION("camera-focal-length"     , decimal           , focalLength     , 0.012                      , found::strtodecimal(optarg), kNoDefaultArgument, REQ_ASSIGN, "The camera focal length (m)"                               ) \
FOUND_CLI_OPTION("camera-pixel-size"       , decimal           , pixelSize       , 20E-6                      , found::strtodecimal(optarg), kNoDefaultArgument, REQ_ASSIGN, "The camera pixel size (m)"                                 ) \
FOUND_CLI_OPTION("reference-orientation"   , found::EulerAngles, refOrientation  , found::EulerAngles(0, 0, 0), found::strtoea(optarg)     , kNoDefaultArgument, REQ_ASSIGN, "The reference orientation (deg)"                           ) \
FOUND_CLI_OPTION("relative-orientation"    , found::EulerAngles, relOrientation  , found::EulerAngles(0, 0, 0), found::strtoea(optarg)     , kNoDefaultArgument, REQ_ASSIGN, "The relative orientation to use (if no calibration) (deg)" ) \
FOUND_CLI_OPTION("planetary-radius"        , decimal           , radius          , DECIMAL_M_R_E              , found::strtodecimal(optarg), kNoDefaultArgument, REQ_ASSIGN, "The planetary radius to use (m)"                           ) \
FOUND_CLI_OPTION("seda-threshold"          , unsigned char     , SEDAThreshold   , 25                         , found::strtouc(optarg)     , kNoDefaultArgument, REQ_ASSIGN, "Threshold for the SEDA Algorithm ([0,255])"                ) \
FOUND_CLI_OPTION("seda-border-len"         , int               , SEDABorderLen   , 1                          , atoi(optarg)               , kNoDefaultArgument, REQ_ASSIGN, "The border thickness for SEDA (pixels)"                    ) \
FOUND_CLI_OPTION("seda-offset"             , decimal           , SEDAOffset      , 0.0                        , found::strtodecimal(optarg), kNoDefaultArgument, REQ_ASSIGN, "The edge offset for SEDA (pixels)"                         ) \
FOUND_CLI_OPTION("output-file"             , std::string       , outputFile      , ""                         , optarg                     , kNoDefaultArgument, REQ_ASSIGN, "The output file (.found)"                                  ) \


// Orbit Flags
// TODO: Fix these all to correct parameters/outputs
#define ORBIT \
FOUND_CLI_OPTION("position-data", found::LocationRecords, positionData, {}           , found::strtolr(optarg)     , kNoDefaultArgument, REQ_ASSIGN, "The position data (.found)"                    ) \
FOUND_CLI_OPTION("output-form"  , std::string           , output      , ""           , optarg                     , kNoDefaultArgument, REQ_ASSIGN, "The desired form of the output"                ) \
FOUND_CLI_OPTION("total-time"   , decimal               , totalTime   , 3600.0       , found::strtodecimal(optarg), kNoDefaultArgument, REQ_ASSIGN, "The total time to predict for (s)"             ) \
FOUND_CLI_OPTION("time-step"    , decimal               , dt          , 0.01         , found::strtodecimal(optarg), kNoDefaultArgument, REQ_ASSIGN, "The time step to use (s)"                      ) \
FOUND_CLI_OPTION("radius"       , decimal               , radius      , DECIMAL_M_R_E, found::strtodecimal(optarg), kNoDefaultArgument, REQ_ASSIGN, "The planetary radius to use (m)"              ) \
FOUND_CLI_OPTION("mu"           , decimal               , mu          , 398600.4418  , found::strtodecimal(optarg), kNoDefaultArgument, REQ_ASSIGN, "The standard gravitational parameter (m^3/s^2)")

// NOLINTEND

class CalibrationOptions {
 public:
#define FOUND_CLI_OPTION(name, type, prop, defaultVal, converter, defaultArg, ASSIGN, doc) \
    type prop = defaultVal;
    CALIBRATION
#undef FOUND_CLI_OPTION
};

class DistanceOptions {
 public:
#define FOUND_CLI_OPTION(name, type, prop, defaultVal, converter, defaultArg, ASSIGN, doc) \
    type prop = defaultVal;
    DISTANCE
#undef FOUND_CLI_OPTION
};

class OrbitOptions {
 public:
#define FOUND_CLI_OPTION(name, type, prop, defaultVal, converter, defaultArg, ASSIGN, doc) \
    type prop = defaultVal;
    ORBIT
#undef FOUND_CLI_OPTION
};

#endif  // OPTIONS_H
