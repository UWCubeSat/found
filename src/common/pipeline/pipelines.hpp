#ifndef SRC_COMMON_PIPELINE_PIPELINES_HPP_
#define SRC_COMMON_PIPELINE_PIPELINES_HPP_

#include <optional>
#include <stdexcept>
#include <cassert>

#include "common/pipeline/stages.hpp"

/// The default number of pipeline stages
#define DEFAULT_NUM_STAGES 10

namespace found {

/**
 * A Pipeline<Input, Output, N> is an abstract Pipeline
 * that takes an Input, outputs an Output, with N stages
 * 
 * @param Input The input type
 * @param Output The output type
 * @param N The number of stages it uses
 * 
 * @pre Output must be able to be default constructed
 */
template<typename Input,
         typename Output,
         size_t N = DEFAULT_NUM_STAGES>
class Pipeline : public FunctionStage<Input, Output> {
 public:
    /**
     * Runs a Pipeline
     * 
     * @note Because Pipelines already construct their
     * return type into the correct destination, there
     * is no need to set the product to the result of the
     * Run, so we optimize here. This override actually
     * isn't necessary for code correctness, but is great
     * for optimization.
     */
    void DoAction() override {
        this->Run(this->resource);
    }

    /**
     * Runs this Pipeline
     * 
     * @param input The input to the Pipeline
     * 
     * @return The output of the Pipeline
     * 
     * @post This method must also construct
     * Output into the correct destination
     * so that Pipeline::GetProduct() may be
     * used
     * 
     * @note In this function only, Pipeline::GetProduct()
     * indicates whether storage is present or not for
     * the output.
     */
    virtual Output Run(const Input &input) = 0;

 protected:
    /// The stages of this
    Action *stages[N];
    /// The number of stages
    size_t size = 0;
    /// Whether we're complete
    bool ready = false;
    /// The final product. This is only sometimes used
    std::optional<Output> finalProduct;

    /**
     * Adds a stage to this pipeline
     * 
     * @param stage The stage to add to the pipeline
     * 
     * @throw std::invalid_argument iff this is already completed
     * 
     * @pre This method is called when the number of registered stages is
     * less than N - 1
     * 
     */
    inline void AddStageHelper(Action &stage) {
        if (this->ready) throw std::invalid_argument("Pipeline is already ready");
        this->stages[size++] = &stage;
    }

    /**
     * Completes a pipeline with a stage
     * 
     * @param stage The stage to add
     * 
     * @throw std::invalid_argument iff this is already completed
     * 
     * @pre This method is called when the number of
     * registered stages is less than N
     * 
     * @pre The pipeline is not ready yet (this->ready == false)
     * 
     * @post The pipeline is ready (this->ready == true)
     */
    inline void CompleteHelper(Action &stage) {
        assert(this->size < N);
        if (this->ready) throw std::invalid_argument("Pipeline is already ready");
        this->stages[size++] = &stage;
        this->ready = true;
    }

    /**
     * Runs the entire pipeline
     * 
     * @pre The pipeline is ready (this->ready == true)
     */
    inline void DoActionHelper() {
        for (size_t i = 0; i < this->size; i++) {
            this->stages[i]->DoAction();
        }
    }
};

/**
 * SequentialPipeline is composite Stage (i.e. A Stage
 * that is made up of many stages). You can
 * add 1 to many stages into a SequentialPipeline so long
 * as the Input to the first stage is the same
 * Input to the SequentialPipeline, and the Output to the
 * last stage is the Output of the SequentialPipeline.
 * 
 * A SequentialPipeline runs by feeding the input to the first
 * stage, taking its output and feeding it to the next
 * stage, and so on until the last stage outputs
 * the output.
 * 
 * @param Input The SequentialPipeline's Input
 * @param Output The SequentialPipeline's Output
 * @param N Number of stages
 * 
 * @pre Output must be able to be default constructed (i.e. output 
 * has trait Output::Output())
 */
template<typename Input, typename Output, size_t N = DEFAULT_NUM_STAGES>
class SequentialPipeline : public Pipeline<Input, Output, N> {
 public:
    /**
     * Constructs an empty SequentialPipeline
     */
    SequentialPipeline() = default;

    /**
     * Adds a stage to this pipeline
     * 
     * @param stage The stage to add to the pipeline
     * 
     * @return this, with the new stage added (for chaining)
     * 
     * @throws invalid_argument iff this is the first time this
     * method is called, and I does not match Input OR if the SequentialPipeline
     * is already complete (aka this::Complete was called successfully)
     * 
     * @pre Iff this method has already been called, O from the last
     * parameter must match I of the next parameter
     * @pre This method is called when the number of registered stages is
     * less than N - 1
     */
    template<typename I, typename O> SequentialPipeline &AddStage(FunctionStage<I, O> &stage) {
        if (this->size == 0) {
            if (!std::is_same<Input, I>::value) {
                throw std::invalid_argument("The initial input type is not correct");
            }
           this->firstResource = reinterpret_cast<Input *>(&stage.GetResource());
        } else {
            // Chain here, and blindly trust the user
            *this->lastProduct = static_cast<void *>(&stage.GetResource());
        }
        // Add to our list
        Pipeline<Input, Output, N>::AddStageHelper(stage);
        // Now, reset the lastProduct to be of this stage
        this->lastProduct = reinterpret_cast<void **>(&stage.GetProduct());
        // Return the pipeline for chaining
        return *this;
    }

    /**
     * Adds a stage to the pipeline and marks it as the last stage,
     * preventing further manipulation of the SequentialPipeline
     * 
     * @param stage The stage to add
     * 
     * @return this, with the last stage added (to run this::Run)
     * 
     * @throws invalid_argument iff this is the first time this
     * method is called, and I does not match Input OR if the SequentialPipeline
     * is already complete (aka this::Complete was called successfully)
     * 
     * @pre This method is called when the number of registered stages is less
     * than N
     */
    template<typename I> SequentialPipeline &Complete(FunctionStage<I, Output> &stage) {
        assert(this->size < N);
        if (this->ready) throw std::invalid_argument("Pipeline is already ready");
        this->AddStage(stage);
        this->ready = true;
        return *this;
    }

    /**
     * Executes this SequentialPipeline
     * 
     * @param input The input to this SequentialPipeline
     * 
     * @return The output of the SequentialPipeline
     * 
     * @note A SequentialPipeline runs by feeding the input to the first
     * stage, taking its output and feeding it to the next
     * stage, and so on until the last stage outputs
     * the output.
     * 
     * @pre The pipeline must have been completed successfully,
     * i.e. before this::Run is called, this::Complete must have
     * been called successfully
     */
    Output Run(const Input &input) override {
        assert(!this->finalProduct);
        // MANUAL VERIFICATION: The below branch is fully tested via pipeline-test.cpp
        if (!this->ready)
            throw std::runtime_error("This is an illegal action: the pipeline is not ready yet");
        // Don't use "Input resource" unless necessary
        *this->firstResource = input;
        // If this pipeline is not composed, construct output
        // into ourself, otherwise construct into the outer pipeline
        if (this->product == nullptr) {
            // engage our finalProduct
            this->finalProduct.emplace();
            *this->lastProduct = &this->finalProduct.value();
            this->product = &this->finalProduct.value();
        } else {
            *this->lastProduct = this->product;
        }
        Pipeline<Input, Output, N>::DoActionHelper();
        return *this->product;
    }

 private:
    /// The pointer to the variable that will store the first input
    Input *firstResource = nullptr;
    /// A temporary variable that always points to the last Stage's product field
    void **lastProduct = nullptr;
};

/**
 * A ModifyingPipeline modifies a resource with a given
 * set of stages
 * 
 * @param T The resource to modify
 * @param N The number of stages that modify the resource
 */
template<typename T, size_t N = DEFAULT_NUM_STAGES>
class ModifyingPipeline : public Pipeline<T, T, N> {
 public:
    /**
     * Constructs a ModifyingPipeline
     */
    ModifyingPipeline() = default;

    /**
     * Adds a stage to this
     * 
     * @param stage The ModifyingStage<T> to add to this
     * 
     * @return this, with the added stage
     */
    ModifyingPipeline &AddStage(ModifyingStage<T> &stage) {
        assert(this->size < N - 1);
        Pipeline<T, T, N>::AddStageHelper(stage);
        return *this;
    }

    /**
     * Completes a pipeline with a stage
     * 
     * @param stage The stage to add
     * 
     * @return this, with the completed pipeline
     * 
     * @pre This method is called when the number of
     * registered stages is less than N
     */
    ModifyingPipeline &Complete(ModifyingStage<T> &stage) {
        assert(this->size < N);
        Pipeline<T, T, N>::CompleteHelper(stage);
        return *this;
    }

    /**
     * Executes this Modifying Pipeline
     * 
     * @param input The input to modify
     * 
     * @return input, but modified by all
     * the registered stages, in order
     */
    T Run(const T &input) override {
        assert(!this->finalProduct);
        if (!this->ready)
            throw std::runtime_error("This is an illegal action: the pipeline is not ready yet");
        // Construct here if there is no next product,
        // or construct there if there is
        if (this->product == nullptr) {
            // engage our finalProduct
            this->finalProduct.emplace();
            this->product = &this->finalProduct.value();
        }
        *this->product = input;
        for (size_t i = 0; i < this->size; i++) {
            dynamic_cast<ModifyingStage<T> *>(this->stages[i])->SetResource(*this->product);
        }
        Pipeline<T, T, N>::DoActionHelper();
        return *this->product;
    }
};

}  // namespace found

#endif  // SRC_COMMON_PIPELINE_PIPELINES_HPP_
