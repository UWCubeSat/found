/**
 * This file defines a mock that we want to use
 * for our example test. We've intentionally been
 * vauge about what a mock is. Essentially, it is a way
 * to set the behavior of a class that is not implemented
 * yet. This is important because often in tests, we need
 * to test a particular class, but not its fields (or,
 * what we normally refer to as the "dependencies of the class")
 * 
 * This allows us to mock the fields of a class, and thus inject
 * them into it via Dependency Injection. However, for the example
 * test case, we are not doing that, and we instead show a very
 * surface level example of how to use a mock
 */

#include <gmock/gmock.h>

#include "src/distance/edge.hpp"
#include "src/style/style.hpp"

namespace found {

/**
 * This class defines a mock. Notice how we need to extend
 * an the class we wish to mock
 */
class MockEdgeDetectionAlgorithm : public EdgeDetectionAlgorithm {
 public:
    // We define the virtual constructor here so the compiler doesn't complain
    virtual ~MockEdgeDetectionAlgorithm() {}
    // Now, we define the method we want to mock. You can read more about
    // what exactly this does, but we are essentially telling gmock to
    // erase any existing behavior of this method so that we can tell
    // this method what to do when its called. More information can
    // be found here: https://google.github.io/googletest/gmock_for_dummies.html
    MOCK_METHOD(Points, Run, (const Image &), (override));
};

}  // namespace found
