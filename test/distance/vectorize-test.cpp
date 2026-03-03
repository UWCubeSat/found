#include <gtest/gtest.h>

#include "test/common/common.hpp"

#include "src/common/spatial/attitude-utils.hpp"
#include "src/common/spatial/camera.hpp"
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
 * and is equivalent to q.conjugate() * v.
 * I write this alternate way as a way to do it
 * in two independent ways.
 */
Vec3 ProjectVector(Vec3 &v, Quaternion &q) {
    Vec3 x = q * Vec3(1, 0, 0);
    Vec3 y = q * Vec3(0, 1, 0);
    Vec3 z = q * Vec3(0, 0, 1);
    return Vec3(v.dot(x)/x.dot(x), v.dot(y)/y.dot(y), v.dot(z)/z.dot(z));
}

TEST(LOSTVectorGenerationAlgorithmTest, TestIdentityRotation) {
    // Setup Dependencies
    Quaternion referenceOrientation = SphericalToQuaternion(0, DECIMAL_M_PI/2, 0).conjugate();
    LOSTVectorGenerationAlgorithm vectorGen(Quaternion::Identity(), referenceOrientation, Quaternion::Identity());

    // Create a PositionVector to test with
    PositionVector x_E = {100.0, 200.0, 300.0};
    PositionVector actual = vectorGen.Run(x_E);

    // Check if the result is as expected
    ASSERT_VEC3_EQ_DEFAULT(-x_E, actual);
}

TEST(LOSTVectorGenerationAlgorithmTest, TestAlongOpticalAxis) {
    // Setup Dependencies
    Quaternion referenceOrientation = SphericalToQuaternion(DECIMAL_M_PI, 0, 0).conjugate();
    LOSTVectorGenerationAlgorithm vectorGen(referenceOrientation, Quaternion::Identity());

    // Create a PositionVector to test with
    PositionVector x_E = {0.0, 0.0, 100.0};
    PositionVector actual = vectorGen.Run(x_E);

    // Check if the result is as expected
    PositionVector expected = {-100.0, 0.0, 0.0};  // Negate x and y, keep z
    ASSERT_VEC3_EQ_DEFAULT(-expected, actual);
}

TEST(LOSTVectorGenerationAlgorithmTest, TestPointAlongVernalEquinox) {
    // Setup Dependencies
    Quaternion referenceOrientation = SphericalToQuaternion(DECIMAL_M_PI, 0, 0).conjugate();
    LOSTVectorGenerationAlgorithm vectorGen(referenceOrientation, Quaternion::Identity());

    // Create a PositionVector to test with
    PositionVector x_E = {1.0, 1.0, 1.0};
    PositionVector actual = vectorGen.Run(x_E);

    // Check if the result is as expected
    PositionVector expected = -1 * PositionVector{-1.0, -1.0, -1.0};  // Negate x and y, keep z
    ASSERT_VEC3_EQ_DEFAULT(expected, actual);
}


// /// Previous TEST


TEST(LOSTVectorGenerationAlgorithmTest, TestIdentityReferenceSimpleTest) {
    // Setup Dependencies
    Quaternion referenceOrientation = SphericalToQuaternion(0, 0, 0).conjugate();
    Quaternion relativeOrientation = SphericalToQuaternion(DECIMAL_M_PI, 0, 0).conjugate();
    LOSTVectorGenerationAlgorithm vectorGen(relativeOrientation, referenceOrientation, Quaternion::Identity());

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
    Quaternion referenceOrientation = SphericalToQuaternion(DECIMAL(DECIMAL_M_PI / 6), 0, 0).conjugate();
    Quaternion relativeOrientation = SphericalToQuaternion(DECIMAL(DECIMAL_M_PI / 12), 0, 0).conjugate();
    LOSTVectorGenerationAlgorithm vectorGen(relativeOrientation, referenceOrientation, Quaternion::Identity());

    // Create a PositionVector to test with
    PositionVector x_E = {100.0, 200.0, 300.0};
    PositionVector actual = vectorGen.Run(x_E);

    // Run(x_E) = -(relOri * refOri * x_E)
    PositionVector expected = -(relativeOrientation * referenceOrientation * x_E);

    ASSERT_VEC3_EQ_DEFAULT(expected, actual);
}

TEST(LOSTVectorGenerationAlgorithmTest, TestSimpleZRotationTest2) {
    // Setup Dependencies
    Quaternion referenceOrientation = SphericalToQuaternion(DECIMAL(DECIMAL_M_PI / 6), 0, 0).conjugate();
    Quaternion relativeOrientation = SphericalToQuaternion(DECIMAL(DECIMAL_M_PI / 6), 0, 0).conjugate();
    LOSTVectorGenerationAlgorithm vectorGen(relativeOrientation, referenceOrientation, Quaternion::Identity());

    // Create a PositionVector to test with
    PositionVector x_E = {100.0, 200.0, 300.0};
    PositionVector actual = vectorGen.Run(x_E);

    // Run(x_E) = -(relOri * refOri * x_E)
    PositionVector expected = -(relativeOrientation * referenceOrientation * x_E);
    ASSERT_VEC3_EQ_DEFAULT(expected, actual);
}

TEST(LOSTVectorGenerationAlgorithmTest, TestRotationIntoAribtraryFrame) {
    // Setup Dependencies
    Quaternion orientation = SphericalToQuaternion(DECIMAL(3.9), DECIMAL(-0.5), DECIMAL(6.1)).conjugate();
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
    Quaternion referenceOrientation = SphericalToQuaternion(DECIMAL(5.9), DECIMAL(1.2), DECIMAL(4.7)).conjugate();
    Quaternion relativeOrientation = SphericalToQuaternion(DECIMAL(5.2), DECIMAL(-0.5), DECIMAL(3.4)).conjugate();
    LOSTVectorGenerationAlgorithm vectorGen(relativeOrientation, referenceOrientation, Quaternion::Identity());

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
    LOSTVectorGenerationAlgorithm vectorGen(relativeOrientation, referenceOrientation, Quaternion::Identity());

    // Create a PositionVector to test with
    PositionVector x_E = {100.0, 200.0, 300.0};
    PositionVector actual = -vectorGen.Run(x_E);

    // Round-trip: applying the full inverse rotation chain to actual should recover x_E
    PositionVector backToX_E = referenceOrientation.conjugate() * relativeOrientation.conjugate() * actual;
    ASSERT_VEC3_EQ_DEFAULT(x_E, backToX_E);
}

TEST(LOSTVectorGenerationAlgorithmTest, TestNonStandardCameraRotation) {
    // Setup Dependencies
    Quaternion referenceOrientation = Quaternion(0, 1.0, 2.0, 3.0).normalized();
    Quaternion relativeOrientation = Quaternion(0, 4.0, 5.0, 6.0).normalized();
    Quaternion cameraEquatorialCoordinateOffset = Quaternion(AngleAxis(DECIMAL(DECIMAL_M_PI / 4), Vec3(1, 0, 0)));
    LOSTVectorGenerationAlgorithm vectorGen(relativeOrientation, referenceOrientation,
                                            cameraEquatorialCoordinateOffset);

    // Create a PositionVector to test with
    PositionVector x_E = {100.0, 200.0, 300.0};
    PositionVector actual = -vectorGen.Run(x_E);

    // Round-trip: applying the full inverse rotation chain to actual should recover x_E
    // orientation = relOri * refOri * camOffset, so inverse = camOffset.conj * refOri.conj * relOri.conj
    PositionVector backToX_E = cameraEquatorialCoordinateOffset.conjugate()
        * referenceOrientation.conjugate()
        * relativeOrientation.conjugate()
        * actual;
    ASSERT_VEC3_EQ_DEFAULT(x_E, backToX_E);
}

}  // namespace found
