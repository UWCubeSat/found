#include "src/common/pipeline.hpp"

#define INIT_PIPELINE(I, O, pipeline, stages) Pipeline<I, O> pipeline(stages)

namespace found {

/**
 * Int->Char Pipeline
 */
#define INIT_INT_TO_CHAR_PIPELINE(pipeline, stages) INIT_PIPELINE(int, char, pipeline, stages)
#define INIT_DOUBLE_TO_STRING_PIPELINE(pipeline, stages) INIT_PIPELINE(double, std::string, pipeline, stages)
#define INIT_CHAR_TO_DOUBLE_PIPELINE(pipeline, stages) INIT_PIPELINE(char, double, pipeline, stages)

constexpr int integers[] = {5, -2, 62};
constexpr const char *strings[] = {"Hello World!", "Foo", "Bar"};
constexpr char characters[] = {'c', 'a', '\n'};
constexpr float floats[] = {5.5, 8.8, -152.1};
constexpr double doubles[] = {-5.1, 23.2, -63.2};

// std {vector, map, lists, struc, vec2, vec3}

}  // namespace found
