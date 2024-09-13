#include <stdexcept>
#include <functional>
#include <utility>

#include "pipeline/pipeline.hpp"

namespace found {

template<typename Input, typename Output>
Stage<Input, Output>::~Stage() {
    if (this->resource != nullptr) delete static_cast<Input *>(this->resource);
    if (this->product != nullptr) delete static_cast<Output *>(this->product);
}

template<typename Input, typename Output>
void Stage<Input, Output>::DoAction() {
    Output output = this->Run(*static_cast<Input *>(this->resource));
    *(static_cast<Output *>(this->product)) = std::move(output);
}

template<typename Input, typename Output>
template<typename I, typename O>
Pipeline<Input, Output> &Pipeline<Input, Output>::AddStage(Stage<I, O> &stage) {
    if (this->ready) throw std::invalid_argument("Pipeline is already ready");
    if (this->stages.empty()) {
        if (!std::is_same<Input, I>::value) throw std::invalid_argument("The initial input type is not correct");
        // Setup the input resource
        stage->resource = static_cast<void *>(new Input());
    } else {
        // Here, we want the input to be the output of the last action
        stage->resource = this->stages.back().get().product;
    }
    // Add to our list
    this->stages.push_back(stage);
    // Finally, setup the output resource
    stage->product = static_cast<void *>(new Output());

    return this;
}

template<typename Input, typename Output>
template<typename I>
Pipeline<Input, Output> &Pipeline<Input, Output>::Complete(Stage<I, Output> &stage) {
    this->ready = true;
    this->AddStage(stage);
    return this;
}

template<typename Input, typename Output>
Output Pipeline<Input, Output>::Run(Input input) {
    if (!this->ready) throw std::runtime_error("This is an illegal action: the pipeline is not ready yet");
    *static_cast<Input *>(this->stages.front().get().resource) = input;
    for (Action &stage : this->stages) {
        stage.DoAction();
    }
    return *static_cast<Output *>(this->stages.back().get().product);
}

}  // namespace found
