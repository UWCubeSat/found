#ifndef PIPELINE_H_
#define PIPELINE_H_

#include <vector>
#include <functional>
#include <stdexcept>
#include <utility>
#include <iostream>

namespace found {

class Action {
 public:
    virtual void DoAction() = 0;
};

/**
 * A Stage is a data structure that
 * wraps a function, and taking in
 * parameter Input and returning Output
 * 
 * @param Input The parameter to accept to the Run function
 * @param Output The parameter to return from the Run function
 */
template<typename Input, typename Output>
class Stage : public Action {
 public:
    Stage() = default;
    virtual ~Stage() = default;
    virtual Output Run(Input input) = 0;
    void DoAction() override {
      *this->product = this->Run(this->resource);
    };
    Input &GetResource() {return resource;}
    Output **GetProduct() {return &product;}
    void SetProduct(Output *ptr) {product = ptr;}

 protected:
    Input resource;
    Output *product;
};

template<typename Input, typename Output>
class Pipeline : public Stage<Input, Output> {
 public:
    explicit Pipeline(std::vector<std::reference_wrapper<Action>> &stages)
       : stages(stages), firstResource(nullptr), ready(false) {}
    template<typename I, typename O> Pipeline &AddStage(Stage<I, O> &stage) {
        // Check the input
        if (this->ready) throw std::invalid_argument("Pipeline is already ready");
        if (this->stages.empty()) {
           if (!std::is_same<Input, I>::value) throw std::invalid_argument("The initial input type is not correct");
           this->firstResource = reinterpret_cast<Input *>(&stage.GetResource());
        } else {
            // Chain here, and blindly trust the user
            *this->lastProduct = static_cast<void *>(&stage.GetResource());
        }
        // Add to our list
        this->stages.push_back(stage);
        // Now, reset the lastProduct to be of this stage
        this->lastProduct = reinterpret_cast<void **>(stage.GetProduct());
        // Return the pipeline for chaining
        return *this;
    }
    template<typename I> Pipeline &Complete(Stage<I, Output> &stage) {
        this->AddStage(stage);
        this->ready = true;
        stage.SetProduct(&this->finalProduct);
        return *this;
    }
    Output Run(Input input) {
        if (!this->ready) throw std::runtime_error("This is an illegal action: the pipeline is not ready yet");
        this->resource = input;
        *this->firstResource = input;
        for (Action &stage : this->stages) {
           stage.DoAction();
        }
        return this->finalProduct;
    }

 private:
    std::vector<std::reference_wrapper<Action>> stages;
    Input *firstResource;
    Output finalProduct;
    void **lastProduct = nullptr;
    bool ready;
};

}  // namespace found

#endif
