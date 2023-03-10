#include <catch.hpp>

#include "edge.hpp"

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
 * 
*/

// This is placed here for convinience, otherwise, we need to "found::" everything. You should
// note this is extremely bad style in most settings, because unless otherwise stated, it assumes
// that everything not defined using a directive or definition originates from the indicated namespace,
// so be careful about utilizing the "using namespace" clause.
using namespace found;

// A global object to be used by all my test methods (I only have one)
SimpleEdgeDetectionAlgorithm eda = SimpleEdgeDetectionAlgorithm();

/**
 * All tests have the method name TEST_CASE. This is really a macro, so it is a preprocessor
 * related statement, and its not actually code (But you better but real code inside of it)
 * 
 * The "parameter you pass in" is the name of this test.
 * 
 * In this case, this test is named "My first test case"
*/
TEST_CASE("My first test case") {
    unsigned char arr[1] = {1};
    Points p = eda.Run(arr);

    /**
     * This is a section, a macro inside of another macro. You can think of it
     * as a mini test within a test, and it also has its own name too that you
     * can pass in as a parameter. You don't actually need this here, you can do
     * what's inside without actually having to enclose it in a section
     * 
     * This section of the test "My first test case" is called "Preliminary (Sanity) Checks"
     * 
    */
    SECTION("Preliminary (Sanity) Checks") {
        // CHECK is another macro. It checks to see if the contained test is true. If it isn't, 
        // the test is marked as failed, but the test will still run everything after.
        CHECK(&p != nullptr);
    }

    SECTION("Content Checks") {

        // This is the REQUIRE macro. It is the same as CHECK, but it will stop the entire
        // test if it fails, and not run after this point. This is basically the equivalent of
        // assert() in C/C++.
        REQUIRE((p.at(0).x == 1 && p.at(0).y == 2));

        CHECK((p.at(1).x == 3 && p.at(1).y == 4));
    }

    CHECK(p.size() == 2);
}

// All other tests are formatted as shown above, and would simply go below