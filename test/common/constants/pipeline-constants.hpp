#include "src/pipeline/pipeline.hpp"

#define INIT_PIPELINE(I, O, pipeline, stages) Pipeline<I, O> pipeline(stages)

namespace found {

/**
 * Int->Char Pipeline
 */
#define INIT_INT_TO_CHAR_PIPELINE(pipeline, stages) INIT_PIPELINE(int, char, pipeline, stages)
#define INIT_DOUBLE_TO_STRING_PIPELINE(pipeline, stages) INIT_PIPELINE(double, std::string, pipeline, stages)
#define INIT_CHAR_TO_DOUBLE_PIPELINE(pipeline, stages) INIT_PIPELINE(char, double, pipeline, stages)

constexpr int integers[] = {5};
constexpr const char *strings[] = {"Hello World!"};
constexpr char characters[] = {'c'};
constexpr float floats[] = {5.5};
constexpr double doubles[] = {5.1};

// std {vector, map, lists, struc, vec2, vec3}

}  // namespace found
