#ifndef TEST_COMMON_CONSTANTS_PIPELINE_CONSTANTS_HPP_
#define TEST_COMMON_CONSTANTS_PIPELINE_CONSTANTS_HPP_

#include "src/common/pipeline/pipelines.hpp"

namespace found {

#define INIT_SQ_PIPELINE(I, O, pipeline) SequentialPipeline<I, O> pipeline
#define INIT_SQ_INT_TO_CHAR_PIPELINE(pipeline) INIT_SQ_PIPELINE(int, char, pipeline)
#define INIT_SQ_DOUBLE_TO_STRING_PIPELINE(pipeline) INIT_SQ_PIPELINE(double, std::string, pipeline)
#define INIT_SQ_CHAR_TO_DOUBLE_PIPELINE(pipeline) INIT_SQ_PIPELINE(char, double, pipeline)

#define INIT_M_PIPELINE(T, pipeline) ModifyingPipeline<T> pipeline
#define INIT_M_INT_PIPELINE(pipeline) INIT_M_PIPELINE(int, pipeline)
#define INIT_M_DOUBLE_PIPELINE(pipeline) INIT_M_PIPELINE(double, pipeline)
#define INIT_M_CHAR_PIPELINE(pipeline) INIT_M_PIPELINE(char, pipeline)

constexpr int integers[] = {5, -2, 62};
constexpr const char *strings[] = {"Hello World!", "Foo", "Bar"};
constexpr char characters[] = {'c', 'a', '\n'};
constexpr float floats[] = {5.5, 8.8, -152.1};
constexpr double doubles[] = {-5.1, 23.2, -63.2};

// std {vector, map, lists, struc, vec2, vec3}

}  // namespace found

#endif  // TEST_COMMON_CONSTANTS_PIPELINE_CONSTANTS_HPP_
