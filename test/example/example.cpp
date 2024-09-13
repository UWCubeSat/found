/**
 * This is an include directive that will allow us to include the Catch framework, which is used
 * to generate test cases for C++ programs
*/
#include <gtest/gtest.h>

/**
 * Include all relavant hpp files here where you need access to specific cpp files. The Makefile
 * is configured to use those files despite being in a seperate folder, so it will be recognized
*/
#include "test/common/constants/example-constants.hpp"

/**
 * We always write our tests in the test folder, which get compiled and run (yes, both) when 
 * we write to the command-line "make test". This format of testing is equivalent to JUnit in
 * a way where all of our tests are methods that use testing-specific methods. Other tests, like
 * script tests, can also go in this folder as well.
*/

/**
 * This is an example of what you might see. Pretend that SimpleEdgeDetectionAlgorithm
 * is a very simple algorithm, and we always know that when we Run() it, it will always
 * spit out a Points object with 2 Points, (1, 2) and (3, 4).
*/

namespace found {

class ExampleTest : public testing::Test {
 protected:
    MockEdgeDetectionAlgorithm *eda;
    void SetUp() override {
        eda = new MockEdgeDetectionAlgorithm();
        EXPECT_CALL(*eda, Run(testing::_))
        .WillOnce(testing::Return(expectedPoints));
    }
    virtual void TearDown() {
        delete eda;
    }
};
/**
 * All tests have the method name TEST_CASE. This is really a macro, so it is a preprocessor
 * related statement, and its not actually code (But you better but real code inside of it)
 * 
 * The "parameter you pass in" is the name of this test.
 * 
 * In this case, this test is named "My first test case"
*/
TEST_F(ExampleTest, MyFirstTest) {
    Points p = eda->Run(arr);

    ASSERT_NE(nullptr, &p);

    // This is the REQUIRE macro. It is the same as CHECK, but it will stop the entire
    // test if it fails, and not run after this point. This is basically the equivalent of
    // assert() in C/C++.
    ASSERT_EQ(expectedPoint1.x, p.at(0).x);
    ASSERT_EQ(expectedPoint1.y, p.at(0).y);

    EXPECT_EQ(expectedPoint2.x, p.at(1).x);
    EXPECT_EQ(expectedPoint2.y, p.at(1).y);

    ASSERT_EQ(expectedSize, p.size());
}

// All other tests are formatted as shown above, and would simply go below

}  // namespace found
