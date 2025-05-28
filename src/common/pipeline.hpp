#ifndef PIPELINE_H_
#define PIPELINE_H_

#include <vector>
#include <functional>
#include <stdexcept>
#include <utility>

namespace found {

/**
 * Action is an interface that
 * wraps a function that does something
 */
class Action {
 public:
    /**
     * Performs some action
     */
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
    /**
     * Constructs a new Stage
     */
    Stage() = default;

    /**
     * Destroys this
     */
    virtual ~Stage() = default;

    /**
     * Runs this stage
     * 
     * @param input The input to the stage
     * 
     * @return The output of this stage
     */
    virtual Output Run(const Input &input) = 0;

    /**
     * Executes Run (with a stored input and storing the output)
     */
    void DoAction() override {  // GCOVR_EXCL_START
      *this->product = this->Run(this->resource);
    };  // GCOVR_EXCL_STOP

    /**
     * Returns the stored input of this
     * 
     * @return The stored input of this
     */
    Input &GetResource() {return resource;}  // GCOVR_EXCL_LINE

    /**
     * Returns the stored output of this
     * 
     * @return The pointer to the stored output
     * of this
     */
    Output *&GetProduct() {return product;}  // GCOVR_EXCL_LINE

 protected:
    /// The stored input for this
    Input resource;
    /// The pointer to the stored output for this
    Output *product;
};

/**
 * Pipeline is composite Stage (i.e. A Stage
 * that is made up of many stages). You can
 * add 1 to many stages into a Pipeline so long
 * as the Input to the first stage is the same
 * Input to the Pipeline, and the Output to the
 * last stage is the Output of the Pipeline.
 * 
 * A Pipeline runs by feeding the input to the first
 * stage, taking its output and feeding it to the next
 * stage, and so on until the last stage outputs
 * the output.
 * 
 * @param Input The Pipeline's Input
 * @param Output The Pipeline's Output
 */
template<typename Input, typename Output>
class Pipeline : public Stage<Input, Output> {
 public:
    /**
     * Constructs a Pipeline
     * 
     * @param stages The vector of stages to provide this object
     */
    explicit Pipeline(std::vector<std::reference_wrapper<Action>> &stages)
       : stages(stages), firstResource(nullptr), ready(false) {}

    /**
     * Constructs an empty Pipeline
     */
    Pipeline() : firstResource(nullptr), ready(false) {}

    /**
     * Adds a stage to this pipeline
     * 
     * @param stage The stage to add to the pipeline
     * 
     * @return this, with the new stage added (for chaining)
     * 
     * @throws invalid_argument iff this is the first time this
     * method is called, and I does not match Input OR if the Pipeline
     * is already complete (aka this::Complete was called successfully)
     * 
     * @pre Iff this method has already been called, O from the last
     * parameter must match I of the next parameter
     */
    template<typename I, typename O> Pipeline &AddStage(Stage<I, O> &stage) {
        // Check the input
        if (this->ready) throw std::invalid_argument("Pipeline is already ready");  // GCOVR_EXCL_LINE
        if (this->stages.empty()) {
            if (!std::is_same<Input, I>::value) {
                throw std::invalid_argument("The initial input type is not correct");  // GCOVR_EXCL_LINE
            }
           this->firstResource = reinterpret_cast<Input *>(&stage.GetResource());
        } else {
            // Chain here, and blindly trust the user
            *this->lastProduct = static_cast<void *>(&stage.GetResource());
        }
        // Add to our list
        this->stages.push_back(stage);
        // Now, reset the lastProduct to be of this stage
        this->lastProduct = reinterpret_cast<void **>(&stage.GetProduct());
        // Return the pipeline for chaining
        return *this;
    }

    /**
     * Adds a stage to the pipeline and marks it as the last stage,
     * preventing further manipulation of the Pipeline
     * 
     * @param stage The stage to add
     * 
     * @return this, with the last stage added (to run this::Run)
     * 
     * @throws invalid_argument iff this is the first time this
     * method is called, and I does not match Input OR if the Pipeline
     * is already complete (aka this::Complete was called successfully)
     */
    template<typename I> Pipeline &Complete(Stage<I, Output> &stage) {
        this->AddStage(stage);
        this->ready = true;
        stage.GetProduct() = &this->finalProduct;
        return *this;
    }

    /**
     * Executes this Pipeline
     * 
     * @param input The input to this Pipeline
     * 
     * @return The output of the Pipeline
     * 
     * @note A Pipeline runs by feeding the input to the first
     * stage, taking its output and feeding it to the next
     * stage, and so on until the last stage outputs
     * the output.
     * 
     * @pre The pipeline must have been completed successfully,
     * i.e. before this::Run is called, this::Complete must have
     * been called successfully
     */
    Output Run(const Input &input) override {
        // MANUAL VERIFICATION: The below branch is fully tested via pipeline-test.cpp
        if (!this->ready)  // GCOVR_EXCL_LINE
            throw std::runtime_error("This is an illegal action: the pipeline is not ready yet");  // GCOVR_EXCL_LINE
        this->resource = input;
        *this->firstResource = input;
        for (Action &stage : this->stages) {
        stage.DoAction();
        }
        this->product = &this->finalProduct;
        return this->finalProduct;
    }

 private:
    /// The stages of this
    std::vector<std::reference_wrapper<Action>> stages;
    /// The pointer to the variable that will store the first input
    Input *firstResource;
    /// The final product (output)
    Output finalProduct;
    /// A temporary variable that always points to the last Stage's product field
    void **lastProduct = nullptr;
    /// An indicator for if this Pipeline is ready
    bool ready;
};

}  // namespace found

#endif
