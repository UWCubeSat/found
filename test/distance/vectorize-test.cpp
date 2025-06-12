#include <gtest/gtest.h>

#include "test/common/common.hpp"

#include "src/common/spatial/attitude-utils.hpp"
#include "src/common/decimal.hpp"

#include "src/distance/vectorize.hpp"

namespace found {

/**
 * Projects a vector in terms of the axes
 * defined by the quaternion.
 * 
 * @param v The vector to rotate
 * @param q The defining quaternion
 * 
 * @return The vector expressed in q's frame (rotated
 * away from the absolute frame)
 * 
 * @note This performs a backwards rotation,
 * and is equivalent to q.Conjugate().Rotate(v).
 * I write this alternate way as a way to do it
 * in two independent ways.
 */
Vec3 ProjectVector(Vec3 &v, Quaternion &q) {
    Vec3 x = q.Rotate({1, 0, 0});
    Vec3 y = q.Rotate({0, 1, 0});
    Vec3 z = q.Rotate({0, 0, 1});
    return {(v*x)/(x*x), (v*y)/(y*y), (v*z)/(z*z)};
}

TEST(LOSTVectorGenerationAlgorithmTest, TestIdentityTest) {
    // Setup Dependencies
    Quaternion referenceOrientation = SphericalToQuaternion(0, 0, 0);
    Quaternion relativeOrientation = SphericalToQuaternion(0, 0, 0);
    LOSTVectorGenerationAlgorithm vectorGen(relativeOrientation, referenceOrientation);

    // Create a PositionVector to test with
    PositionVector x_E = {100.0, 200.0, 300.0};
    PositionVector actual = vectorGen.Run(x_E);

    // Check if the result is as expected
    ASSERT_VEC3_EQ_DEFAULT(-x_E, actual);
}

TEST(LOSTVectorGenerationAlgorithmTest, TestIdentityReferenceSimpleTest) {
    // Setup Dependencies
    Quaternion referenceOrientation = SphericalToQuaternion(0, 0, 0);
    Quaternion relativeOrientation = SphericalToQuaternion(DECIMAL_M_PI, 0, 0);
    LOSTVectorGenerationAlgorithm vectorGen(relativeOrientation, referenceOrientation);

    // Create a PositionVector to test with
    PositionVector x_E = {100.0, 200.0, 300.0};
    PositionVector actual = vectorGen.Run(-x_E);

    // Check if the result is as expected
    PositionVector expected = {-100, -200.0, 300.0};  // Negate x and y, keep z
    ASSERT_VEC3_EQ_DEFAULT(expected, actual);
}

TEST(LOSTVectorGenerationAlgorithmTest, TestSimpleZRotationTest1) {
    // Setup Dependencies
    Quaternion referenceOrientation = SphericalToQuaternion(DECIMAL(DECIMAL_M_PI / 6), 0, 0);
    Quaternion relativeOrientation = SphericalToQuaternion(DECIMAL(DECIMAL_M_PI / 12), 0, 0);
    LOSTVectorGenerationAlgorithm vectorGen(relativeOrientation, referenceOrientation);

    // Create a PositionVector to test with
    PositionVector x_E = {100.0, 200.0, 300.0};
    PositionVector actual = vectorGen.Run(-x_E);

    PositionVector expected = {100.0 * DECIMAL_COS(DECIMAL_M_PI / 4) + 200.0 * DECIMAL_SIN(DECIMAL_M_PI / 4),
                               -100.0 * DECIMAL_SIN(DECIMAL_M_PI / 4) + 200.0 * DECIMAL_COS(DECIMAL_M_PI / 4),
                               300.0};

    ASSERT_VEC3_EQ_DEFAULT(expected, actual);
}

TEST(LOSTVectorGenerationAlgorithmTest, TestSimpleZRotationTest2) {
    // Setup Dependencies
    Quaternion referenceOrientation = SphericalToQuaternion(DECIMAL(DECIMAL_M_PI / 6), 0, 0);
    Quaternion relativeOrientation = SphericalToQuaternion(DECIMAL(DECIMAL_M_PI / 6), 0, 0);
    LOSTVectorGenerationAlgorithm vectorGen(relativeOrientation, referenceOrientation);

    // Create a PositionVector to test with
    PositionVector x_E = {100.0, 200.0, 300.0};
    PositionVector actual = vectorGen.Run(x_E);

    PositionVector expected = {-100.0 * DECIMAL_COS(DECIMAL_M_PI / 3) - 200.0 * DECIMAL_SIN(DECIMAL_M_PI / 3),
                               100.0 * DECIMAL_SIN(DECIMAL_M_PI / 3) - 200.0 * DECIMAL_COS(DECIMAL_M_PI / 3),
                               -300.0};
    ASSERT_VEC3_EQ_DEFAULT(expected, actual);
}

TEST(LOSTVectorGenerationAlgorithmTest, TestRotationIntoAribtraryFrame) {
    // Setup Dependencies
    Quaternion orientation = SphericalToQuaternion(DECIMAL(3.9), DECIMAL(-0.5), DECIMAL(6.1));
    LOSTVectorGenerationAlgorithm vectorGen(orientation);

    // Create PositionVector to test with
    PositionVector x_E = {92.5, -152.1, 529.2};

    // Get the expected vector
    PositionVector expected = ProjectVector(x_E, orientation);

    // Obtain the result and test
    PositionVector actual = vectorGen.Run(-x_E);

    ASSERT_VEC3_EQ_DEFAULT(expected, actual);
}

TEST(LOSTVectorGenerationAlgorithmTest, TestRotationIntoArbitraryReferenceAndRelativeFrames) {
    // Setup Dependencies
    Quaternion referenceOrientation = SphericalToQuaternion(DECIMAL(5.9), DECIMAL(1.2), DECIMAL(4.7));
    Quaternion relativeOrientation = SphericalToQuaternion(DECIMAL(5.2), DECIMAL(-2.9), DECIMAL(3.4));
    LOSTVectorGenerationAlgorithm vectorGen(relativeOrientation, referenceOrientation);

    // TODO: What happens here spooks me out. We have to take the conjugate here
    // to get the axis vectors, but not in the above (test case). Inside LOSTVectorGenerationAlgorithm,
    // the constructor in this case doesn't conjugate this, but the constructor above does.
    // and we get the expected result in both cases!!!
    Quaternion newOrientation = (relativeOrientation * referenceOrientation).Conjugate();

    // Create PositionVector to test with
    PositionVector x_E = {915.2, 1692.6, -2962.2};

    // Get the expected vector
    PositionVector expected = ProjectVector(x_E, newOrientation);

    PositionVector actual = vectorGen.Run(-x_E);

    ASSERT_VEC3_EQ_DEFAULT(expected, actual);
}

TEST(LOSTVectorGenerationAlgorithmTest, TestGeneral) {
    // Setup Dependencies
    Quaternion referenceOrientation({1.0, 2.0, 3.0});
    Quaternion relativeOrientation({4.0, 5.0, 6.0});
    LOSTVectorGenerationAlgorithm vectorGen(relativeOrientation, referenceOrientation);

    // Create a PositionVector to test with
    PositionVector x_E = {100.0, 200.0, 300.0};
    PositionVector actual = vectorGen.Run(x_E);

    // Should be equivalent to this:
    PositionVector expected = (QuaternionToDCM(relativeOrientation) * QuaternionToDCM(referenceOrientation)) * -x_E;
    ASSERT_VEC3_EQ_DEFAULT(expected, actual);

    // Also, if we take the result and apply the inverse, we should get the original
    PositionVector inverseResult = (QuaternionToDCM(relativeOrientation)
                                        * QuaternionToDCM(referenceOrientation)).Inverse()
                                    * -actual;
    ASSERT_VEC3_EQ_DEFAULT(inverseResult, x_E);
}

}  // namespace found
