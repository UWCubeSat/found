#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <math.h>

#include "src/spatial/attitude-utils.hpp"

namespace found {
    decimal max_difference = 0.0001;  // epsilon value for float comparison

    // Values for IdentityTest
    Vec3 test_vector_1(-5, -5, -5);
    Attitude ref_att_1(SphericalToQuaternion(0, 0, 0));
    Attitude dif_att_1(SphericalToQuaternion(0, 0, 0));
    LOSTVectorGenerationAlgorithm lvga_1(ref_att_1, dif_att_1);

    // Values for 90DegTest
    Vec3 test_vector_2(100, 0, 0);
    Attitude ref_att_2(SphericalToQuaternion(0, 0, 0));
    Attitude dif_att_2(SphericalToQuaternion(0, 0, 0));
    LOSTVectorGenerationAlgorithm lvga_2(ref_att_2, dif_att_2);
    Vec3 result_vector_2(0, 100, 0);
}  // namespace found
