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
    Quaternion q_conj = q.Conjugate();
    Vec3 x = q_conj.Rotate({1, 0, 0});
    Vec3 y = q_conj.Rotate({0, 1, 0});
    Vec3 z = q_conj.Rotate({0, 0, 1});
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

    PositionVector expected = {x_E.x * DECIMAL_COS(DECIMAL_M_PI / 4) - x_E.y * DECIMAL_SIN(DECIMAL_M_PI / 4),
                               x_E.x * DECIMAL_SIN(DECIMAL_M_PI / 4) + x_E.y * DECIMAL_COS(DECIMAL_M_PI / 4),
                               x_E.z};

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

    PositionVector expected = {-x_E.x * DECIMAL_COS(DECIMAL_M_PI / 3) + x_E.y * DECIMAL_SIN(DECIMAL_M_PI / 3),
                               -x_E.x * DECIMAL_SIN(DECIMAL_M_PI / 3) - x_E.y * DECIMAL_COS(DECIMAL_M_PI / 3),
                               -x_E.z};
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

    Quaternion newOrientation = (relativeOrientation * referenceOrientation);

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
}

}  // namespace found
