#ifndef SRC_COMMON_PIPELINE_STAGES_HPP_
#define SRC_COMMON_PIPELINE_STAGES_HPP_

#include <type_traits>

namespace found {

/// Type Alias Template for resolving any type parameter
/// into its non-const non-lvalue
template<typename T>
using raw_type = std::remove_cv_t<std::remove_reference_t<T>>;

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
 * Stage is an interface that
 * wraps an SISO function
 * 
 * @param Input The input type
 * @param Output The output type
 * 
 * @pre Input must be able to be default constructed
 */
template<typename Input, typename Output>
class Stage : public Action {
 public:
    /**
     * Runs this stage
     * 
     * @param input The input to the stage
     * 
     * @return The output of this stage
     */
    virtual Output Run(Input input) = 0;
};

/**
 * A FunctionStage is a data structure that
 * wraps a function, and taking in
 * parameter Input and returning Output
 * 
 * @param Input The parameter to accept to the Run function
 * @param Output The parameter to return from the Run function
 */
template<typename Input, typename Output>
class FunctionStage : public Stage<const raw_type<Input> &, raw_type<Output>> {
 public:
    /**
     * Constructs a new Stage
     */
    FunctionStage() = default;

    /**
     * Destroys this
     */
    virtual ~FunctionStage() = default;

    /**
     * Executes Run (with a stored input and storing the output)
     * 
     * @pre this->product points to a valid location
     */
    void DoAction() override {
      *this->product = this->Run(this->resource);
    };

    /**
     * Returns the stored input of this
     * 
     * @return The stored input of this
     */
    Input &GetResource() {return resource;}

    /**
     * Returns the stored output of this
     * 
     * @return The pointer to the stored output
     * of this
     */
    Output *&GetProduct() {return product;}

 protected:
    /// The stored input for this
    Input resource;
    /// The pointer to the stored output for this
    Output *product = nullptr;
};

/**
 * ModifyingStage is a stage that modifies a resource
 * 
 * @param T The type of resource being modified
 */
template<typename T>
class ModifyingStage : public Stage<raw_type<T> &, void> {
 public:
    /**
     * Constructs a new ModifyingStage
     */
    ModifyingStage() = default;

    /**
     * Destroys this
     */
    virtual ~ModifyingStage() = default;

    /**
     * Executes Run (with void return)
     */
    void DoAction() override {
        this->Run(*this->resource);
    };

    /**
     * Sets the resource to modify
     * 
     * @param resource The resource to modify
     * 
     * @post When used in a ModifyingPipeline,
     * this will modify resource via Run(T &)
     */
    void SetResource(T &resource) {this->resource = &resource;}

 private:
    /// The pointer to the resource
    T *resource = nullptr;
};

}  // namespace found


#endif  // SRC_COMMON_PIPELINE_STAGES_HPP_
