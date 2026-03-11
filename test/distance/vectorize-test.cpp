#include <gtest/gtest.h>

#include "test/common/common.hpp"

#include "src/common/spatial/attitude-utils.hpp"
#include "src/common/spatial/camera.hpp"
#include "src/common/decimal.hpp"

#include "src/distance/vectorize.hpp"

namespace found {

TEST(LOSTVectorGenerationAlgorithmTest, TestIdentityRotation) {
    // Setup Dependencies
    Quaternion referenceOrientation = SphericalToQuaternion(0, 0, 0);
    LOSTVectorGenerationAlgorithm vectorGen(Quaternion::Identity(), referenceOrientation);

    // Create a PositionVector to test with
    PositionVector x_E = {100.0, 200.0, 300.0};
    PositionVector actual = vectorGen.Run(x_E);

    // Check if the result is as expected
    ASSERT_VEC3_EQ_DEFAULT(-x_E, actual);
}

TEST(LOSTVectorGenerationAlgorithmTest, TestRightAscensionRotation) {
    // Setup Dependencies
    Quaternion referenceOrientation = SphericalToQuaternion(DECIMAL_M_PI, 0, 0);
    LOSTVectorGenerationAlgorithm vectorGen(referenceOrientation, Quaternion::Identity());

    // Create a PositionVector to test with
    PositionVector x_E = {100.0, 0.0, 100.0};
    PositionVector actual = vectorGen.Run(x_E);

    // Check if the result is as expected
    PositionVector expected = {-100.0, 0.0, 100.0};  // Negate x and y, keep z
    ASSERT_VEC3_EQ_DEFAULT(-expected, actual);
}

TEST(LOSTVectorGenerationAlgorithmTest, TestNorthPoleRotation) {
    // Setup Dependencies
    Quaternion referenceOrientation = SphericalToQuaternion(0, DECIMAL_M_PI / 2, 0);
    LOSTVectorGenerationAlgorithm vectorGen(referenceOrientation, Quaternion::Identity());

    // Create a PositionVector to test with
    PositionVector x_E = {1.0, 2.0, 3.0};
    PositionVector actual = vectorGen.Run(x_E);

    // Check if the result is as expected
    PositionVector expected = {-3.0,2.0,1.0};  // Negate x and y, keep z
    ASSERT_VEC3_EQ_DEFAULT(-expected, actual);
}

TEST(LOSTVectorGenerationAlgorithmTest, TestIdentityReferenceSimpleTest) {
    // Setup Dependencies
    Quaternion referenceOrientation = SphericalToQuaternion(0, 0, 0);
    Quaternion relativeOrientation = SphericalToQuaternion(DECIMAL_M_PI, 0, 0);
    LOSTVectorGenerationAlgorithm vectorGen(relativeOrientation, referenceOrientation);

    // Create a PositionVector to test with
    PositionVector x_E = {100.0, 200.0, 300.0};
    PositionVector actual = vectorGen.Run(x_E);

    // Check if the result is as expected
    // Run(x_E) = -(relOri * refOri * x_E)
    PositionVector expected = -(relativeOrientation * referenceOrientation * x_E);
    ASSERT_VEC3_EQ_DEFAULT(expected, actual);
}

TEST(LOSTVectorGenerationAlgorithmTest, TestSimpleZRotationTest1) {
    // Setup Dependencies
    Quaternion referenceOrientation = SphericalToQuaternion(DECIMAL(DECIMAL_M_PI / 6), 0, 0);
    Quaternion relativeOrientation = SphericalToQuaternion(DECIMAL(DECIMAL_M_PI / 12), 0, 0);
    LOSTVectorGenerationAlgorithm vectorGen(relativeOrientation, referenceOrientation);

    // Create a PositionVector to test with
    PositionVector x_E = {100.0, 200.0, 300.0};
    PositionVector actual = vectorGen.Run(x_E);

    // Run(x_E) = -(relOri * refOri * x_E)
    PositionVector expected = -(relativeOrientation * referenceOrientation * x_E);

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

    // Run(x_E) = -(relOri * refOri * x_E)
    PositionVector expected = -(relativeOrientation * referenceOrientation * x_E);
    ASSERT_VEC3_EQ_DEFAULT(expected, actual);
}

TEST(LOSTVectorGenerationAlgorithmTest, TestRotationIntoAribtraryFrame) {
    // Setup Dependencies
    Quaternion orientation = SphericalToQuaternion(DECIMAL(3.9), DECIMAL(-0.5), DECIMAL(6.1));
    LOSTVectorGenerationAlgorithm vectorGen(orientation, Quaternion::Identity());

    // Create PositionVector to test with
    PositionVector x_E = {92.5, -152.1, 529.2};

    // Run(x_E) = -(orientation * x_E)
    PositionVector expected = -(orientation * x_E);

    // Obtain the result and test
    PositionVector actual = vectorGen.Run(x_E);

    ASSERT_VEC3_EQ_DEFAULT(expected, actual);
}

TEST(LOSTVectorGenerationAlgorithmTest, TestRotationIntoArbitraryReferenceAndRelativeFrames) {
    // Setup Dependencies
    Quaternion referenceOrientation = SphericalToQuaternion(DECIMAL(5.9), DECIMAL(1.2), DECIMAL(4.7));
    Quaternion relativeOrientation = SphericalToQuaternion(DECIMAL(5.2), DECIMAL(-0.5), DECIMAL(3.4));
    LOSTVectorGenerationAlgorithm vectorGen(relativeOrientation, referenceOrientation);

    Quaternion newOrientation = (relativeOrientation * referenceOrientation);

    // Create PositionVector to test with
    PositionVector x_E = {915.2, 1692.6, -2962.2};

    // Run(x_E) = -(newOrientation * x_E)
    PositionVector expected = -(newOrientation * x_E);

    PositionVector actual = vectorGen.Run(x_E);

    ASSERT_VEC3_EQ_DEFAULT(expected, actual);
}

TEST(LOSTVectorGenerationAlgorithmTest, TestGeneral) {
    // Setup Dependencies
    Quaternion referenceOrientation = Quaternion(0, 1.0, 2.0, 3.0).normalized();
    Quaternion relativeOrientation = Quaternion(0, 4.0, 5.0, 6.0).normalized();
    LOSTVectorGenerationAlgorithm vectorGen(relativeOrientation, referenceOrientation);

    // Create a PositionVector to test with
    PositionVector x_E = {100.0, 200.0, 300.0};
    PositionVector actual = -vectorGen.Run(x_E);

    // Round-trip: applying the full inverse rotation chain to actual should recover x_E
    PositionVector backToX_E = referenceOrientation.conjugate() * relativeOrientation.conjugate() * actual;
    ASSERT_VEC3_EQ_DEFAULT(x_E, backToX_E);
}

///////////////////
//// OLD TESTS ////
///////////////////

/*
 * The following tests are the old tests that were used to test the LOSTVectorGenerationAlgorithm before the direction
 * of rotation was switched-- pre PR #49 (commit 73e090afde7ab8d21f264ef2b50583e3b4a21418).
 * In the newer implementation both the order of SphericalToQuaternion and the order of multiplication in 
 * LOSTVectorGenerationAlgorithm::Run were reversed. These changes cancel eacother out so no changes besides minor syntax
 * have been made.
 */

TEST(LOSTVectorGenerationAlgorithmTest, TestOldIdentityTest) {
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

TEST(LOSTVectorGenerationAlgorithmTest, TestOldIdentityReferenceSimpleTest) {
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

TEST(LOSTVectorGenerationAlgorithmTest, TestOldSimpleZRotationTest1) {
    // Setup Dependencies
    Quaternion referenceOrientation = SphericalToQuaternion(DECIMAL(DECIMAL_M_PI / 6), 0, 0);
    Quaternion relativeOrientation = SphericalToQuaternion(DECIMAL(DECIMAL_M_PI / 12), 0, 0);
    LOSTVectorGenerationAlgorithm vectorGen(relativeOrientation, referenceOrientation);

    // Create a PositionVector to test with
    PositionVector x_E = {100.0, 200.0, 300.0};
    PositionVector actual = vectorGen.Run(-x_E);

    PositionVector expected = {x_E.x() * DECIMAL_COS(DECIMAL_M_PI / 4) - x_E.y() * DECIMAL_SIN(DECIMAL_M_PI / 4),
                               x_E.x() * DECIMAL_SIN(DECIMAL_M_PI / 4) + x_E.y() * DECIMAL_COS(DECIMAL_M_PI / 4),
                               x_E.z()};

    ASSERT_VEC3_EQ_DEFAULT(expected, actual);
}

TEST(LOSTVectorGenerationAlgorithmTest, TestOldSimpleZRotationTest2) {
    // Setup Dependencies
    Quaternion referenceOrientation = SphericalToQuaternion(DECIMAL(DECIMAL_M_PI / 6), 0, 0);
    Quaternion relativeOrientation = SphericalToQuaternion(DECIMAL(DECIMAL_M_PI / 6), 0, 0);
    LOSTVectorGenerationAlgorithm vectorGen(relativeOrientation, referenceOrientation);

    // Create a PositionVector to test with
    PositionVector x_E = {100.0, 200.0, 300.0};
    PositionVector actual = vectorGen.Run(x_E);

    PositionVector expected = {-x_E.x() * DECIMAL_COS(DECIMAL_M_PI / 3) + x_E.y() * DECIMAL_SIN(DECIMAL_M_PI / 3),
                               -x_E.x() * DECIMAL_SIN(DECIMAL_M_PI / 3) - x_E.y() * DECIMAL_COS(DECIMAL_M_PI / 3),
                               -x_E.z()};
    ASSERT_VEC3_EQ_DEFAULT(expected, actual);
}

}  // namespace found
