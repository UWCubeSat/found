#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "test/common/common.hpp"

#include "src/common/spatial/attitude-utils.hpp"
#include "src/common/decimal.hpp"

#include "src/distance/vectorize.hpp"

namespace found {

TEST(LOSTVectorGenerationAlgorithmTest, TestIdentityTest) {
    // Setup Dependencies
    Quaternion referenceOrientation = SphericalToQuaternion(0, 0, 0);
    Quaternion relativeOrientation = SphericalToQuaternion(0, 0, 0);
    LOSTVectorGenerationAlgorithm vectorGen(relativeOrientation, referenceOrientation);

    // Create a PositionVector to test with
    PositionVector x_E = {100.0, 200.0, 300.0};
    PositionVector result = vectorGen.Run(x_E);

    // Check if the result is as expected
    ASSERT_VEC3_EQ_DEFAULT(result, -x_E);
}

TEST(LOSTVectorGenerationAlgorithmTest, TestIdentityReferenceSimpleTest) {
    // Setup Dependencies
    Quaternion referenceOrientation = SphericalToQuaternion(0, 0, 0);
    Quaternion relativeOrientation = SphericalToQuaternion(DECIMAL_M_PI, 0, 0);
    LOSTVectorGenerationAlgorithm vectorGen(relativeOrientation, referenceOrientation);

    // Create a PositionVector to test with
    PositionVector x_E = {100.0, 200.0, 300.0};
    PositionVector result = vectorGen.Run(x_E);

    // Check if the result is as expected
    PositionVector expected = {100, 200.0, -300.0};  // Negate x and y, keep z
    ASSERT_VEC3_EQ_DEFAULT(result, expected);  // Negate x and y, keep z
}

TEST(LOSTVectorGenerationAlgorithmTest, TestSimpleZRotationTest1) {
    // Setup Dependencies
    Quaternion referenceOrientation = SphericalToQuaternion(DECIMAL(DECIMAL_M_PI / 6), 0, 0);
    Quaternion relativeOrientation = SphericalToQuaternion(DECIMAL(DECIMAL_M_PI / 12), 0, 0);
    LOSTVectorGenerationAlgorithm vectorGen(relativeOrientation, referenceOrientation);

    // Create a PositionVector to test with
    PositionVector x_E = {100.0, 200.0, 300.0};
    PositionVector result = vectorGen.Run(x_E);

    PositionVector expected = {-100.0 * cos(DECIMAL_M_PI / 4) - 200.0 * sin(DECIMAL_M_PI / 4),
                               100.0 * sin(DECIMAL_M_PI / 4) - 200.0 * cos(DECIMAL_M_PI / 4),
                               -300.0};
    ASSERT_VEC3_EQ_DEFAULT(result, expected);  // Negate x and y, keep z
}

TEST(LOSTVectorGenerationAlgorithmTest, TestSimpleZRotationTest2) {
    // Setup Dependencies
    Quaternion referenceOrientation = SphericalToQuaternion(DECIMAL(DECIMAL_M_PI / 6), 0, 0);
    Quaternion relativeOrientation = SphericalToQuaternion(DECIMAL(DECIMAL_M_PI / 6), 0, 0);
    LOSTVectorGenerationAlgorithm vectorGen(relativeOrientation, referenceOrientation);

    // Create a PositionVector to test with
    PositionVector x_E = {100.0, 200.0, 300.0};
    PositionVector result = vectorGen.Run(x_E);

    PositionVector expected = {-100.0 * cos(DECIMAL_M_PI / 3) - 200.0 * sin(DECIMAL_M_PI / 3),
                               100.0 * sin(DECIMAL_M_PI / 3) - 200.0 * cos(DECIMAL_M_PI / 3),
                               -300.0};
    ASSERT_VEC3_EQ_DEFAULT(result, expected);
}

TEST(LOSTVectorGenerationAlgorithmTest, TestGeneral) {
    // Setup Dependencies
    Quaternion referenceOrientation({1.0, 2.0, 3.0});
    Quaternion relativeOrientation({4.0, 5.0, 6.0});
    LOSTVectorGenerationAlgorithm vectorGen(relativeOrientation, referenceOrientation);

    // Create a PositionVector to test with
    PositionVector x_E = {100.0, 200.0, 300.0};
    PositionVector result = vectorGen.Run(x_E);

    // Should be equivalent to this:
    PositionVector expected = (QuaternionToDCM(relativeOrientation) * QuaternionToDCM(referenceOrientation)) * -x_E;
    ASSERT_VEC3_EQ_DEFAULT(result, expected);

    // Also, if we take the result and apply the inverse, we should get the original
    PositionVector inverseResult = (QuaternionToDCM(relativeOrientation)
                                        * QuaternionToDCM(referenceOrientation)).Inverse()
                                    * -result;
    ASSERT_VEC3_EQ_DEFAULT(inverseResult, x_E);
}

}  // namespace found
