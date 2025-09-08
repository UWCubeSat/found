#ifndef TEST_COMMON_MOCKS_ORBIT_MOCKS_HPP_
#define TEST_COMMON_MOCKS_ORBIT_MOCKS_HPP_

#include <gmock/gmock.h>

#include "src/orbit/orbit.hpp"

namespace found {

class MockOrbitPropagationAlgorithm : public OrbitPropagationAlgorithm {
 public:
    // We define the virtual constructor here so the compiler doesn't complain
    virtual ~MockOrbitPropagationAlgorithm() {}
    // Now, we define the method we want to mock. You can read more about
    // what exactly this does, but we are essentially telling gmock to
    // erase any existing behavior of this method so that we can tell
    // this method what to do when its called. More information can
    // be found here: https://google.github.io/googletest/gmock_for_dummies.html
    MOCK_METHOD(LocationRecords, Run, (const LocationRecords &), (override));
};

}  // namespace found

#endif  // TEST_COMMON_MOCKS_ORBIT_MOCKS_HPP_
