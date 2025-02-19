#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "src/spatial/attitude-utils.hpp"

namespace found {
    Vec3 test_vector_1(-5, -5, -5);
    Attitude ref_att_1(SphericalToQuaternion(0, 0, 0));
    Attitude dif_att_1(SphericalToQuaternion(0, 0, 0));
    LOSTVectorGenerationAlgorithm lvga1(ref_att_1, dif_att_1);
}  // namespace found
