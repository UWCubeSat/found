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
#include "test/distance/distance-test/testData.hpp"

using found::Camera;
using found::Vec3;
using found::Vec2;
using found::Mat3;
using found::Points;
using found::PositionVector;
using found::SpheroidDistanceDeterminationAlgorithm;
using found::bigAssImage;

#define BALLPARK 0.001 // testing analytical solutions; should be precise to floating point error

// The equatorial radius of Earth (m)
#define RADIUS_OF_EARTH_A (static_cast<decimal>(6378.1366))
// The polar radius of Earth (m)
#define RADIUS_OF_EARTH_C (static_cast<decimal>(6356.7519))

#define VECTOR_EQUALS(vec1, vec2, tolerance) \
    EXPECT_LT(abs(vec1.x - vec2.x), tolerance); \
    EXPECT_LT(abs(vec1.y - vec2.y), tolerance); \
    EXPECT_LT(abs(vec1.z - vec2.z), tolerance);



TEST(SpheroidDistanceDeterminationAlgorithmTest, TestRandomEarth1) {
    Vec3 principleAxisDimensions = {RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_C};
    int imageWidth = 700;
    int imageHeight = 600;
    Camera cam(0.05, .0000514, imageWidth, imageHeight); 
    Vec3 AOR = {-0.16754666,  0.28948027, -0.94240612};
    Points pts = {{static_cast<decimal>(677.5358753014197), static_cast<decimal>(510.15044202539)},{static_cast<decimal>(645.4936938723756), static_cast<decimal>(537.8532693624234)},{static_cast<decimal>(666.8925017023865), static_cast<decimal>(519.512917282545)},{static_cast<decimal>(571.6925155472582), static_cast<decimal>(596.4595861829636)},{static_cast<decimal>(650.229882251296), static_cast<decimal>(533.8486928114402)},{static_cast<decimal>(653.4711543060477), static_cast<decimal>(531.0903381321456)},{static_cast<decimal>(583.2444742484045), static_cast<decimal>(587.7384580802305)},{static_cast<decimal>(624.0162819052008), static_cast<decimal>(555.6329035292347)},{static_cast<decimal>(690.0881275883015), static_cast<decimal>(498.89791667430774)},{static_cast<decimal>(595.7197721001701), static_cast<decimal>(578.1371751716015)},{static_cast<decimal>(654.2496467562646), static_cast<decimal>(530.425671137898)},{static_cast<decimal>(603.9238800113577), static_cast<decimal>(571.7175135288273)},{static_cast<decimal>(627.2821922796454), static_cast<decimal>(552.9688895751124)},{static_cast<decimal>(647.3614673111051), static_cast<decimal>(536.2776843167289)},{static_cast<decimal>(674.5449888460973), static_cast<decimal>(512.7978029753647)},{static_cast<decimal>(692.2624233665705), static_cast<decimal>(496.92519609493945)},{static_cast<decimal>(686.1580981320584), static_cast<decimal>(502.44585062483765)},{static_cast<decimal>(584.422851297312), static_cast<decimal>(586.8397423024081)},{static_cast<decimal>(610.9917589996783), static_cast<decimal>(566.1186642910959)},{static_cast<decimal>(594.6884000765087), static_cast<decimal>(578.9382493445212)},{static_cast<decimal>(577.064420433236), static_cast<decimal>(592.4241359293442)},{static_cast<decimal>(623.7793506281705), static_cast<decimal>(555.8256237233293)},{static_cast<decimal>(682.48407597481), static_cast<decimal>(505.74212401109503)},
};
    SpheroidDistanceDeterminationAlgorithm algo(std::move(cam), principleAxisDimensions, AOR);
    Vec3 actual = algo.Run(pts);
    Vec3 expected = {2758.45431576228, 3689.6447536370315, -5824.432376674428};

    Vec3 trueRp = {-4214.133704672528, -489.8481180046763, 6094.8902427420635};

    Mat3 computedTCP = algo.ComputeBodyToCamTransformation(AOR);
    Mat3 analyticalTCP = {-0.9336827385619191, 0.3164880078507422, -0.16754666394226866,
-0.3534677092449802, -0.8895289487459533, 0.2894802719772545,
0.05742057325228543, -0.32950506859468437, -0.9424061160337359,
};
    Vec3 computedRp = analyticalTCP * expected;

    std::stringstream ss1;
    ss1 << "\nTCP Det: " << computedTCP.Det() << ", " << analyticalTCP.Det();
    ss1 << "\nTCP computed dist: [" << computedRp.x << ", "<< computedRp.y << ", " << computedRp.z << "]; " << computedRp.MagnitudeSq();
    ss1 << "\nTrue dist: [" << trueRp.x << ", "<< trueRp.y << ", " << trueRp.z << "]; " << trueRp.MagnitudeSq();
    computedRp = computedTCP * expected;
    ss1 << "\nTCP computed dist: [" << computedRp.x << ", "<< computedRp.y << ", " << computedRp.z << "]; " << computedRp.MagnitudeSq();
    ss1 << "\nActual rc: [" << actual.x << ", "<< actual.y << ", " << actual.z << "]";
    ss1 << "\nexpected rc: [" << expected.x << ", "<< expected.y << ", " << expected.z << "]";
    LOG_INFO(ss1.str());

    VECTOR_EQUALS(actual, expected, BALLPARK);
}