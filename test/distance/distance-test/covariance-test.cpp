#include <gtest/gtest.h>

#include <cmath>
#include <utility>
#include <string>
#include <memory>

// Test for SpheroidDistanceAndCovarianceAlgorithm
#include "distance/distance.hpp"
#include "common/decimal.hpp"
#include "common/spatial/camera.hpp"
#include "common/spatial/attitude-utils.hpp"
#incl

using found::Camera;
using found::Vec3;
using found::Vec2;
using found::Mat3;
using found::Points;
using found::PositionVector;
using found::SpheroidDistanceDeterminationAlgorithm;
using found::SpheroidDistanceAndCovarianceDeterminationAlgorithm;
using found::Quarternion;


// The equatorial radius of Earth (m)
#define RADIUS_OF_EARTH_A (DECIMAL(6378.1366))
// The polar radius of Earth (m)
#define RADIUS_OF_EARTH_C (DECIMAL(6356.7519))

TEST(SpheroidDistanceAndCovarianceAlgorithm, SymmetricCovarianceOutput) {
    // create a camera
    Camera cam(DECIMAL(0.005), 4000, 3000, DECIMAL(2000), DECIMAL(1500), DECIMAL(1.12e-6), DECIMAL(1.12e-6));
    // define principle axes of the spheroid
    Vec3 principleAxes(RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_C);
    // calculate the orientation of the camera relative to the spheroid - thi is a matrix?
    Quaternion relativeOrientation = found::AttitudeUtils::CalculateRelativeOrientation(
        Vec3(1, 0, 0), // camera is looking in the positive x direction
        Vec3(0, 1, 0), // camera's up direction is in the positive y direction
        Vec3(0, 0, 1)  // camera's right direction is in the positive z direction
    );
    
    // create a spheriod distance determination algorithm class to pass into the covariance algorithm class
    SpheroidDistanceDeterminationAlgorithm distanceAlgorithm(std::move(cam), principleAxes, relativeOrientation, relativeOrientation);
    
    // create a spheriod distance and covariance determination algorithm class
    SpheroidDistanceAndCovarianceDeterminationAlgorithm covarianceAlgorithm(std::make_unique<SpheroidDistanceDeterminationAlgorithm>(std::move(distanceAlgorithm)));
    
    // create a set of points on the horizon of a celestial body
    Points points = {
        Vec2(1000, 1500),
        Vec2(2000, 1500),
        Vec2(3000, 1500)
    };

    // run the algorithm on the set of points
    auto result = covarianceAlgorithm.Run(points);

    // test that the covariance is symmetric
    EXPECT_TRUE(result.covariance.isApprox(result.covariance.transpose()));
}