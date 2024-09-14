#include "src/pipeline/pipeline.hpp"

#define INIT_PIPELINE(I, O, pipeline, stages) Pipeline<I, O> pipeline(stages)

namespace found {

/**
 * Int->Char Pipeline
 */
#define INIT_INT_TO_CHAR_PIPELINE(pipeline, stages) INIT_PIPELINE(int, char, pipeline, stages)

constexpr int integers[] = {5};
constexpr const char *strings[] = {"Hello World!"};
constexpr char characters[] = {'c'};

}  // namespace found
