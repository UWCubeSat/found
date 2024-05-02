#include "attitude-utils.hpp"
#include "style.hpp"
#include "camera.hpp"
#include "distance.hpp"
#include <math.h>

namespace found {
    class SphericalDistanceDeterminationAlgorithm {
    public:         
        distFromEarth Run(char *image, Points &p, int imageWidth, int imageHeight)
        {
            return solve(p, radius_);
        }
    private:

        Vec3 getCenter(Vec3* spats){
            Vec3 diff1 = spats[1] - spats[0];
            Vec3 diff2 = spats[2] -spats[1];

            Vec3 circleN = diff1.CrossProduct(diff2);
            Vec3 circlePt = spats[0];

            Vec3 mid1 = midpoint(spats[0], spats[1]);
            Vec3 mid2 = midpoint(spats[1], spats[2]);

            Vec3 mid1N = diff1;
            Vec3 mid2N = diff2;

            Mat3 matrix;
            matrix.x[0] = circleN.x;
            matrix.x[1] = circleN.y;
            matrix.x[2] = circleN.z;
            matrix.x[3] = mid1N.x;
            matrix.x[4] = mid1N.y;
            matrix.x[5] = mid1N.z;
            matrix.x[6] = mid2N.x;
            matrix.x[7] = mid2N.y;
            matrix.x[8] = mid2N.z; 

            int alpha = circleN*circlePt;
            int beta = mid1N*mid1;
            int gamma = mid2N*mid2;

            Vec3 y;
            y.x = alpha;
            y.y = beta;
            y.z = gamma;

            Vec3 center = y * matrix.Inverse();

            return center;
        }

        decimal getRadius(Vec3* spats, Vec3 center){
            return Distance(spats[0], center);
        }

        decimal getDistance(decimal r)
        {
            return 6378/r;
        }

        distFromEarth solve(Points& pts, int R){
            Vec3 spats[3] = {cam_.CameraToSpatial(pts[0]), cam_.CameraToSpatial(pts[1]), cam_.CameraToSpatial(pts[2])};
            Vec3 uSpats[3] = {spats[0].Normalize(), spats[1].Normalize(), spats[2].Normalize()};

            Vec3 center = getCenter(uSpats);
            decimal r = getRadius(uSpats, center);

            decimal h = getDistance(r);

            printf("========= solution =============\n");
            printf("((center), h) = ((%f, %f, %f), %f)\n", center.x,center.y,center.z,r);
            printf("Distance from surface: %f\n", h - R);

            return center * h;
        }

        
    };
}
