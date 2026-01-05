#include <gtest/gtest.h>

#include <cmath>
#include <utility>

#include "common/style.hpp"
#include "common/spatial/attitude-utils.hpp"
#include "common/spatial/camera.hpp"
#include "distance/distance.hpp"
#include "common/logging.hpp"
#include "common/decimal.hpp"
#include "test/common/common.hpp"


using found::Vec3;
using found::Mat3;
/**
 * Requires that vec1 == vec2 (using DecimalEquals)
 *
 * @param vec1 A Vec3 object
 * @param vec2 A Vec3 object
 * @param tolerance The tolerance for vec1 to be
 * "equal" to vec2
 *
 * @post Will have REQUIRE'd that vec1 is equal to
 * vec2, on a component basis, within tolerance
*/
#define VECTOR_EQUALS(vec1, vec2, tolerance) \
    EXPECT_LT(abs(vec1.x - vec2.x), tolerance); \
    EXPECT_LT(abs(vec1.y - vec2.y), tolerance); \
    EXPECT_LT(abs(vec1.z - vec2.z), tolerance);

void checkIndividualEigenvectorDependence(Vec3 expected, Vec3 actual, decimal tolerance){

    if (abs(expected.x) < tolerance || abs(actual.x) < tolerance){
        EXPECT_LT(abs(expected.x - actual.x), tolerance);
        if (abs(expected.y) < tolerance || abs(actual.y) < tolerance){
            EXPECT_LT(abs(expected.y - actual.y), tolerance);
            if (abs(expected.z) < tolerance || abs(actual.z) < tolerance) { EXPECT_LT(abs(expected.z-actual.z), tolerance); }
        } else {
            decimal ratio = expected.y / actual.y;
            if (abs(expected.z) < tolerance || abs(actual.z) < tolerance) { EXPECT_LT(abs(expected.z - actual.z), tolerance);
            } else EXPECT_LT(abs(expected.z/actual.z - ratio), tolerance);
        }
    } else {
        decimal ratio = expected.x / actual.x;
        if (abs(expected.y) < tolerance || abs(actual.y) < tolerance) { EXPECT_LT(abs(expected.y - actual.y), tolerance);
        } else EXPECT_LT(abs(expected.y/actual.y - ratio), tolerance);
        if (abs(expected.z) < tolerance || abs(actual.z) < tolerance) { EXPECT_LT(abs(expected.z - actual.z), tolerance);
        } else EXPECT_LT(abs(expected.z/actual.z - ratio), tolerance);
    }
}


void checkMatrixEigenvectorDependence(Mat3 expected, Mat3 actual, decimal tolerance){
    // Log eigenvector matrix
    // for (int i = 0; i < 3; i++){
    //     std::stringstream ss1;
    //     ss1 <<  "expected: " << expected.Column(i).x << " " << expected.Column(i).y << " " << expected.Column(i).z << "; actual : " << actual.Column(i).x << " " << actual.Column(i).y << " " << actual.Column(i).z;
    //     LOG_INFO(ss1.str());
    // }
    checkIndividualEigenvectorDependence(expected.Column(0), actual.Column(0), tolerance);
    checkIndividualEigenvectorDependence(expected.Column(1), actual.Column(1), tolerance);
    checkIndividualEigenvectorDependence(expected.Column(2), actual.Column(2), tolerance);
}

TEST(SymmetricMat3EigenanalysisTest, TestIdentityMatrix) {
    // three eigenvalues of 1 are expected for the identity matrix
    Vec3 expected = {static_cast<decimal>(1.0),
                                static_cast<decimal>(1.0),
                                static_cast<decimal>(1.0)};

    // Not const because eigenvalues get stored
    Mat3 IdentityMat3 = { 1,0,0,
                          0,1,0,
                          0,0,1};

    Vec3 actual = IdentityMat3.EigenvaluesSymmetric();
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);

    // ordering is arbitrary so idk how to check
    // Mat3 actualMat = IdentityMat3.EigenvectorsSymmetric();
    // Mat3 expectedMat = IdentityMat3;
    // checkMatrixEigenvectorDependence(expectedMat, actualMat, DEFAULT_TOLERANCE);
}

TEST(SymmetricMat3EigenanalysisTest, TestScaledIdentityMatrix) {
    // three eigenvalues of 1 are expected for the identity matrix
    Vec3 expected = {static_cast<decimal>(4.0),
                                static_cast<decimal>(3.0),
                                static_cast<decimal>(2.0)};

    // Not const because eigenvalues get stored
    Mat3 input = { 4,0,0,
                   0,3,0,
                   0,0,2};

    Vec3 actual = input.EigenvaluesSymmetric();
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);

    Mat3 actualMat = input.EigenvectorsSymmetric();
    Mat3 expectedMat = input;
    checkMatrixEigenvectorDependence(expectedMat, actualMat, DEFAULT_TOLERANCE);
}

TEST(SymmetricMat3EigenanalysisTest, TestReverseScaledIdentityMatrix) {
    // three eigenvalues of 1 are expected for the identity matrix
    Vec3 expected = {static_cast<decimal>(4.0),
                                static_cast<decimal>(3.0),
                                static_cast<decimal>(2.0)};

    // Not const because eigenvalues get stored
    Mat3 input = { 2,0,0,
                   0,3,0,
                   0,0,4};

    Vec3 actual = input.EigenvaluesSymmetric();
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);

    Mat3 actualMat = input.EigenvectorsSymmetric();
    Mat3 expectedMat = { 0,0,1,
                         0,1,0,
                         1,0,0};
    checkMatrixEigenvectorDependence(expectedMat, actualMat, DEFAULT_TOLERANCE);
}

TEST(SymmetricMat3EigenanalysisTest, TestRandomMatrix) {
    // three eigenvalues of 1 are expected for the identity matrix
    Vec3 expected = {static_cast<decimal>(6.1439),
                                static_cast<decimal>(1.38445),
                                static_cast<decimal>(-1.52835)};

    // Not const because eigenvalues get stored
    Mat3 input = {  1, 2, 3,
                    2, 2, 1,
                    3, 1, 3};

    Vec3 actual = input.EigenvaluesSymmetric();
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);

    Mat3 actualMat = input.EigenvectorsSymmetric();
    Mat3 expectedMat = { 0.573927,  -0.0200101, -0.818662,
                         0.443177,  -0.833065,   0.331054,
                         0.688623,   0.552813,   0.469251 };
    checkMatrixEigenvectorDependence(expectedMat, actualMat, DEFAULT_TOLERANCE);
}

TEST(SymmetricMat3EigenanalysisTest, TestEnvelopeMatrix) {
    // three eigenvalues of 1 are expected for the identity matrix
    Vec3 expected = {static_cast<decimal>(23.2845),
                                static_cast<decimal>(11.8807),
                                static_cast<decimal>(-6.69974)};

    // Not const because eigenvalues get stored
    Mat3 input = {   12.9597, 11.9471, -7.76076,
                    11.9471, 6.19767, 3.82588,
                    -7.76076, 3.82588, 9.30806};

    Vec3 actual = input.EigenvaluesSymmetric();
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);

    Mat3 actualMat = input.EigenvectorsSymmetric();
    Mat3 expectedMat = { -0.809697,  -0.00903044, 0.586779,
                         -0.495862,  0.5453,   -0.675847,
                         0.313868, 0.838192,   0.44605 };
    checkMatrixEigenvectorDependence(expectedMat, actualMat, DEFAULT_TOLERANCE);
}





// THE ALGORITHM DOES NOT WORK IF AN EIGENVALUE IS 0; I DON'T THINK WE NEED THIS THOUGH

TEST(SymmetricMat3EigenanalysisTest, TestEigenvalue0Matrix) {
    // three eigenvalues of 1 are expected for the identity matrix
    Vec3 expected = {static_cast<decimal>(10.8151),
                                static_cast<decimal>(0.184927),
                                static_cast<decimal>(0)};

    // Not const because eigenvalues get stored
    Mat3 input = {  9, 4, 0,
                    4, 2, 0,
                    0, 0, 0};

    Vec3 actual = input.EigenvaluesSymmetric();
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);

    Mat3 actualMat = input.EigenvectorsSymmetric();
    Mat3 expectedMat = {  0.910633,  -0.413216, 0,
                         0.413216,   0.910633,   0,
                         0,   0,   1 };
    checkMatrixEigenvectorDependence(expectedMat, actualMat, DEFAULT_TOLERANCE);
}