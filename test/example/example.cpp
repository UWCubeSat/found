/**
 * This is an include directive that will allow us to include the gtest framework
*/
#include <gtest/gtest.h>
/**
 * This is an include directive that will allow us to include the gmock framework.
 * This is only needed when you need to create Mocks of not-yet-implemented classes
 * or methods. You will see the use of Mocks throughout this test case, but feel free
 * to ignore any syntax related to gmock, especially EXPECT_CALL, WillOnce, or Mock*
 */
#include <gmock/gmock.h>

/**
 * Here, we include our testing constants for this test class.
 * This is convention and should be followed. More information
 * can be found in this class
 */
#include "test/common/constants/example-constants.hpp"

/**
 * This is an example of what you might see. Here, we make an example test that tests
 * a Mock EdgeDetectionAlgorithm, that always returns the same result
*/
namespace found {

/**
 * This class defines the resources needed
 * across test cases, but also defines
 * external behavior as well.
 * 
 * In the gtest framework, this is called
 * a "Test Fixture"
 * 
 * You may be wondering, why isn't this in
 * the test constants? That's because this class
 * is specific to this test file, wheras the constants
 * might not necessarily be limited to this file
 */
class ExampleTest : public testing::Test {
 protected:
    // This is a common EdgeDetectionAlgorithm
    // that we'd like to use
    MockEdgeDetectionAlgorithm *eda;
    /**
     * This method always runs before TEST_F(ExampleTest, ...)
     * test cases, and can be used to setup our mock.
     * Here, we initialize our field to a Mock EdgeDetectionAlgorithm
     */
    void SetUp() override {
        eda = new MockEdgeDetectionAlgorithm();
    }
    /**
     * This method always runs after TEST_F(ExampleTest, ...)
     * test cases, and can be used to destroy any heap objects.
     * Here, we delete our mock.
     */
    virtual void TearDown() {
        delete eda;
    }
};

/**
 * Google's Testing Framework (gtest) forces you to define all tests in the same test file
 * with the same header. The list of headers is here: https://google.github.io/googletest/reference/testing.html
 * 
 * For this example test, we've defined a test fixture, and we want to use it. Thus, we must
 * go for TEST_F (F is for Fixture) over TEST (which is generic). Notice that even with a fixture,
 * we've defined our test cases to be outside of the fixture (which is a bit strange, but it doesn't
 * work like JUnit where it uses Reflection to figure out all the test methods in a Test class)
 * 
 * Thus, all tests in this file have the method name TEST_F. This is really a macro, so it is a preprocessor
 * related statement, and its not actually code (But you better but real code inside of it)
 * 
 * The "parameters you pass in" is firstly the name of the test fixure (that we've defined earlier),
 * and a descriptor of what the test is testing
*/
TEST_F(ExampleTest, MyFirstTest) {
    // Here, we have an ASSERT statment. There are many different
    // types of assertions that you can use, which can be found
    // here: https://google.github.io/googletest/reference/assertions.html
    ASSERT_NE(1, 0);
}

/**
 * Here, we have a second test, which actually puts our test
 * fixture to use
 */
TEST_F(ExampleTest, MySecondTest) {
    // Ignore unless interested: First, we define the behavior of the mock
    // More information about what's happening can be found within
    // test/common/mocks/example-mocks.hpp, or at the following website:
    // https://google.github.io/googletest/gmock_for_dummies.html
    EXPECT_CALL(*eda, Run(testing::_))
        .WillOnce(testing::Return(expectedPoints));

    // Here, we run the function we want to test,
    // which will execute our Mocked behavior
    Points p = eda->Run(arr);

    // Now, we test if the result of our
    // function matched our expectation
    ASSERT_EQ(expectedPoint1.x, p.at(0).x);
    ASSERT_EQ(expectedPoint1.y, p.at(0).y);

    // This is EXPECT_*. It parallels ASSERT_*
    // statments, but unlike ASSERT_*, EXPECT_*
    // continues the test case even if the condition
    // inside fails. However, this will still fail
    // the entire test case
    EXPECT_EQ(expectedPoint2.x, p.at(1).x);
    EXPECT_EQ(expectedPoint2.y, p.at(1).y);

    ASSERT_EQ(expectedSize, p.size());
}

// All other tests are formatted as shown above, and would simply go below

}  // namespace found
