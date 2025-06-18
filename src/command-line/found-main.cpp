#include "command-line/found-main.hpp"

#include <iostream>
#include <memory>
#include <string>

#include "common/logging.hpp"
#include "common/style.hpp"
#include "providers/factory.hpp"

#include "command-line/execution/executors.hpp"
#include "command-line/parsing/parser.hpp"
#include "command-line/parsing/options.hpp"

namespace found {

int main(int argc, char **argv) {
    if (argc == 1) {
        LOG_ERROR("Seems you don't want to be found. " << HELP_MSG);
        return EXIT_FAILURE;
    }
    std::string command(argv[1]);

    std::unique_ptr<PipelineExecutor> executor;
    if (command == "calibration") {
        executor = CreateCalibrationPipelineExecutor(ParseCalibrationOptions(argc, argv));
    } else if (command == "distance") {
        executor = CreateDistancePipelineExecutor(ParseDistanceOptions(argc, argv));
    // TODO: Uncomment when orbit stage is implemented
    // } else if (command == "orbit") {
    //     executor = CreateOrbitPipelineExecutor(ParseOrbitOptions(argc, argv));
    } else if (command == "--help" || command == "-h") {
        // Usage and primary details
        std::cout << "Usage: ./found <option> [[flag value]...] [[flag=value]...]" << std::endl;
        std::cout << std::endl;
        std::cout << "Finds absolute position of Images relative to Earth "
                << "and projects for orbit given multiple images. " << std::endl
                << "Current Capabilities include: " << std::endl;
        std::cout << "\t1. Calibrates the algorithm to produce a relative attitude (option: calibration)" << std::endl;
        std::cout << "\t2. Finds the distance from a given image to a planet (option: distance)" << std::endl;
        // TODO: Uncomment when orbit stage is implemented
        // std::cout << "\t3. Projects an orbit from multiple position vectors (option: distance)" << std::endl;
        std::cout << std::endl;

        // Flag usage details
        std::cout << "==================== Calibration Flags ====================" << std::endl;
        std::cout << std::endl;
        #define FOUND_CLI_OPTION(name, type, prop, defaultVal, converter, defaultArg, ASSIGN, doc) \
                std::cout << "    --" << name << std::endl;                                        \
                std::cout << "\t\t" << doc << std::endl;
        CALIBRATION
        #undef FOUND_CLI_OPTION
        std::cout << std::endl;
        std::cout << "==================== Distance Flags ====================" << std::endl;
        std::cout << std::endl;
        #define FOUND_CLI_OPTION(name, type, prop, defaultVal, converter, defaultArg, ASSIGN, doc) \
                std::cout << "    --" << name << std::endl;                                        \
                std::cout << "\t\t" << doc << std::endl;
        DISTANCE
        #undef FOUND_CLI_OPTION
        // TODO: Uncomment when orbit stage is implemented
        // std::cout << std::endl;
        // std::cout << "==================== Orbit Flags ====================" << std::endl;
        // std::cout << std::endl;
        /*
        #define FOUND_CLI_OPTION(name, type, prop, defaultVal, converter, defaultArg, ASSIGN, doc) \
                std::cout << "\t\t--" << name << std::endl;                                        \
                std::cout << "\t\t\t\t--" << doc << std::endl;                                     \
        ORBIT
        #undef FOUND_CLI_OPTION
        */
        return EXIT_SUCCESS;
    } else {
        LOG_ERROR("Unrecognized Command: " << command << ". " << HELP_MSG);
        return EXIT_FAILURE;
    }

    executor->ExecutePipeline();
    executor->OutputResults();

    return EXIT_SUCCESS;
}

}  // namespace found
