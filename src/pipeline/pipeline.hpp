#ifndef PIPELINE_H_
#define PIPELINE_H_

#include <vector>
#include <functional>

namespace found {

class Action {
 public:
    virtual void DoAction() = 0;
    Action() : resource(nullptr), product(nullptr) {}

 protected:
    void *resource;
    void *product;
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
    Stage() {}
    virtual ~Stage();
    virtual Output Run(Input input) = 0;
    virtual void DoAction();
};

template<typename Input, typename Output>
class Pipeline : public Stage<Input, Output> {
 public:
    explicit Pipeline(std::vector<std::reference_wrapper<Action>> &stages) : stages(stages), ready(false) {}
    template<typename I, typename O> Pipeline &AddStage(Stage<I, O> &stage);
    template<typename I> Pipeline &Complete(Stage<I, Output> &stage);
    Output Run(Input input);
 private:
    std::vector<std::reference_wrapper<Action>> stages;
    bool ready;
};

}  // namespace found

#endif
