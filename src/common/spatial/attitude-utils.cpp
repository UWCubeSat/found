
#include "common/spatial/attitude-utils.hpp"

#include <math.h>
#include <assert.h>

#include <stdexcept>

namespace found {

///////////////////////////////////
///////// VECTOR CLASSES //////////
///////////////////////////////////

decimal Vec2::Magnitude() const {
    return sqrt(MagnitudeSq());
}

decimal Vec2::MagnitudeSq() const {
    return x*x+y*y;
}

Vec2 Vec2::Normalize() const {
    decimal mag = Magnitude();
    return {
        x/mag, y/mag,
    };
}

decimal Vec2::operator*(const Vec2 &other) const {
    return x*other.x + y*other.y;
}

Vec2 Vec2::operator*(const decimal &scalar) const {
    return { x*scalar, y*scalar };
}

Vec2 Vec2::operator+(const Vec2 &other) const {
    return {x + other.x, y + other.y };
}

Vec2 Vec2::operator-(const Vec2 &other) const {
    return { x - other.x, y - other.y };
}

decimal Vec3::Magnitude() const {
    return sqrt(MagnitudeSq());
}

decimal Vec3::MagnitudeSq() const {
    return x*x+y*y+z*z;
}

Vec3 Vec3::Normalize() const {
    decimal mag = Magnitude();
    return {
        x/mag, y/mag, z/mag,
    };
}

Vec3 Vec3::operator-() const {
    return { -x, -y, -z };
}

Vec3 Vec3::operator-(const Vec3 &other) const {
    return { x - other.x, y - other.y, z - other.z };
}


decimal Vec3::operator*(const Vec3 &other) const {
    return x*other.x + y*other.y + z*other.z;
}

Vec3 Vec3::operator*(const decimal &scalar) const {
    return { x*scalar, y*scalar, z*scalar };
}

Vec3 Vec3::operator/(const decimal &divisor) const {
    return { x / divisor, y / divisor, z / divisor };
}

Vec3 &Vec3::operator+=(const Vec3 &other) {
    this->x += other.x;
    this->y += other.y;
    this->z += other.z;

    return *this;
}

Vec3 Vec3::operator*(const Mat3 &other) const {
    return {
        x*other.At(0,0) + y*other.At(0,1) + z*other.At(0,2),
        x*other.At(1,0) + y*other.At(1,1) + z*other.At(1,2),
        x*other.At(2,0) + y*other.At(2,1) + z*other.At(2,2),
    };
}

Vec3 Vec3::CrossProduct(const Vec3 &other) const {
    return {
        y*other.z - z*other.y,
        -(x*other.z - z*other.x),
        x*other.y - y*other.x,
    };
}

Mat3 Vec3::OuterProduct(const Vec3 &other) const {
    return {
        x*other.x, x*other.y, x*other.z,
        y*other.x, y*other.y, y*other.z,
        z*other.x, z*other.y, z*other.z
    };
}

///////////////////////////////////
///// VECTOR UTILITY FUNCTIONS ////
///////////////////////////////////

Vec2 Midpoint(const Vec2 &vec1, const Vec2 &vec2) {
    return {(vec1.x + vec2.x)/2, (vec1.y + vec2.y)/2};
}

Vec3 Midpoint(const Vec3 &vec1, const Vec3 &vec2) {
    return {(vec1.x + vec2.x)/2, (vec1.y + vec2.y)/2, (vec1.z + vec2.z)/2};
}

Vec3 Midpoint(const Vec3 &vec1, const Vec3 &vec2, const Vec3 &vec3) {
    return {(vec1.x + vec2.x + vec3.x)/3, (vec1.y + vec2.y + vec3.y)/3, (vec1.z + vec2.z + vec3.z)/3};
}

decimal Distance(const Vec2 &v1, const Vec2 &v2) {
    return (v1-v2).Magnitude();
}

decimal Distance(const Vec3 &v1, const Vec3 &v2) {
    return (v1-v2).Magnitude();
}

decimal Angle(const Vec3 &vec1, const Vec3 &vec2) {
    return AngleUnit(vec1.Normalize(), vec2.Normalize());
}

decimal AngleUnit(const Vec3 &vec1, const Vec3 &vec2) {
    decimal dot = vec1*vec2;
    // TODO: we shouldn't need this nonsense, right? how come acos sometimes gives nan?
    return dot >= 1 ? 0 : dot <= -1 ? DECIMAL_M_PI-DECIMAL(0.0000001) : DECIMAL_ACOS(dot);
}

///////////////////////////////////
///////// MATRIX CLASS ////////////
///////////////////////////////////

decimal Mat3::At(int i, int j) const {
    return x[3*i+j];
}

Vec3 Mat3::Column(int j) const {
    return {At(0,j), At(1,j), At(2,j)};
}

Vec3 Mat3::Row(int i) const {
    return {At(i,0), At(i,1), At(i,2)};
}

decimal Mat3::Trace() const {
    return At(0,0) + At(1,1) + At(2,2);
}

decimal Mat3::Det() const {
    return (At(0,0) * (At(1,1)*At(2,2) - At(2,1)*At(1,2))) -
    (At(0,1) * (At(1,0)*At(2,2) - At(2,0)*At(1,2))) +
    (At(0,2) * (At(1,0)*At(2,1) - At(2,0)*At(1,1)));
}

Mat3 Mat3::operator+(const Mat3 &other) const {
    return {
        At(0,0)+other.At(0,0), At(0,1)+other.At(0,1), At(0,2)+other.At(0,2),
        At(1,0)+other.At(1,0), At(1,1)+other.At(1,1), At(1,2)+other.At(1,2),
        At(2,0)+other.At(2,0), At(2,1)+other.At(2,1), At(2,2)+other.At(2,2)
    };
}

Mat3 Mat3::operator*(const Mat3 &other) const {
#define _MATMUL_ENTRY(row, col) At(row,0)*other.At(0,col) + At(row,1)*other.At(1,col) + At(row,2)*other.At(2,col)
    return {
        _MATMUL_ENTRY(0,0), _MATMUL_ENTRY(0,1), _MATMUL_ENTRY(0,2),
        _MATMUL_ENTRY(1,0), _MATMUL_ENTRY(1,1), _MATMUL_ENTRY(1,2),
        _MATMUL_ENTRY(2,0), _MATMUL_ENTRY(2,1), _MATMUL_ENTRY(2,2),
    };
#undef _MATMUL_ENTRY
}

Vec3 Mat3::operator*(const Vec3 &vec) const {
    return {
        vec.x*At(0,0) + vec.y*At(0,1) + vec.z*At(0,2),
        vec.x*At(1,0) + vec.y*At(1,1) + vec.z*At(1,2),
        vec.x*At(2,0) + vec.y*At(2,1) + vec.z*At(2,2),
    };
}

Mat3 Mat3::operator*(const decimal &scalar) const {
    return {
        scalar*At(0,0), scalar*At(0,1), scalar*At(0,2),
        scalar*At(1,0), scalar*At(1,1), scalar*At(1,2),
        scalar*At(2,0), scalar*At(2,1), scalar*At(2,2)
    };
}

Mat3 Mat3::Transpose() const {
    return {
        At(0,0), At(1,0), At(2,0),
        At(0,1), At(1,1), At(2,1),
        At(0,2), At(1,2), At(2,2),
    };
}

Mat3 Mat3::Inverse() const {
    // https://ardoris.wordpress.com/2008/07/18/general-formula-for-the-inverse-of-a-3x3-matrix/
    decimal scalar = 1 / Det();

    Mat3 res = {
        At(1,1)*At(2,2) - At(1,2)*At(2,1), At(0,2)*At(2,1) - At(0,1)*At(2,2), At(0,1)*At(1,2) - At(0,2)*At(1,1),
        At(1,2)*At(2,0) - At(1,0)*At(2,2), At(0,0)*At(2,2) - At(0,2)*At(2,0), At(0,2)*At(1,0) - At(0,0)*At(1,2),
        At(1,0)*At(2,1) - At(1,1)*At(2,0), At(0,1)*At(2,0) - At(0,0)*At(2,1), At(0,0)*At(1,1) - At(0,1)*At(1,0)
    };

    return res * scalar;
}

/// 3x3 identity matrix
const Mat3 kIdentityMat3 = {1,0,0,
                            0,1,0,
                            0,0,1};

///////////////////////////////////
///////// QUATERNION CLASS ////////
///////////////////////////////////

/**
 * Create a quaternion which represents a rotation of theta around the axis input
 * 
 * @param input the axis about which theta is defined
 * @param theta the rotation about the input
*/
Quaternion::Quaternion(const Vec3 &input, decimal theta) {
    real = cos(theta/2);
    // the compiler will optimize it. Right?
    i = input.x * sin(theta/2);
    j = input.y * sin(theta/2);
    k = input.z * sin(theta/2);
}

Quaternion::Quaternion(const Vec3 &input) {
    real = 0;
    SetVector(input);
}

Quaternion Quaternion::operator*(const Quaternion &other) const {
    return Quaternion(
        real*other.real - i*other.i - j*other.j - k*other.k,
        real*other.i + other.real*i + j*other.k - k*other.j,
        real*other.j + other.real*j + k*other.i - i*other.k,
        real*other.k + other.real*k + i*other.j - j*other.i);
}

Quaternion Quaternion::operator-() const {
    return Quaternion(-real, -i, -j, -k);
}

Quaternion Quaternion::Conjugate() const {
    return Quaternion(real, -i, -j, -k);
}

Vec3 Quaternion::Vector() const {
    return { i, j, k };
}

void Quaternion::SetVector(const Vec3 &vec) {
    i = vec.x;
    j = vec.y;
    k = vec.z;
}

Vec3 Quaternion::Rotate(const Vec3 &input) const {
    // TODO: optimize
    return ((*this)*Quaternion(input)*Conjugate()).Vector();
}

decimal Quaternion::Angle() const {
    if (real <= -1) {
        return 0;  // 180*2=360=0
    }
    // TODO: we shouldn't need this nonsense, right? how come acos sometimes gives nan? (same as in AngleUnit)
    return (real >= 1 ? 0 : acos(real))*2;
}

void Quaternion::SetAngle(decimal newAngle) {
    real = cos(newAngle/2);
    SetVector(Vector().Normalize() * sin(newAngle/2));
}

bool Quaternion::IsUnit(decimal tolerance) const {
    return abs(i*i+j*j+k*k+real*real - 1) < tolerance;
}

Quaternion Quaternion::Canonicalize() const {
    if (real >= 0) {
        return *this;
    }

    return Quaternion(-real, -i, -j, -k);
}

EulerAngles Quaternion::ToSpherical() const {
    // Working out these equations would be a pain in the ass. Thankfully, this wikipedia page:
    // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Quaternion_to_Euler_angles_conversion
    // uses almost exactly the same euler angle scheme that we do, so we copy their equations almost
    // wholesale. The only differences are that 1, we use de and roll in the opposite directions,
    // and 2, so we invert the de and roll terms.
    //
    // WARNING: This function was modified from LOST's version, which assumed that this Quaternion was a
    // backwards quaternion instead of forwards, which simply placed a negative sign in front of the real
    // terms in the usages below
    decimal ra = atan2(2*(real*k+i*j), 1-2*(j*j+k*k));
    if (ra < 0)
        ra += 2*DECIMAL_M_PI;
    decimal de = -asin(2*(real*j-i*k));  // allow de to be positive or negaive, as is convention
    decimal roll = -atan2(2*(real*i+j*k), 1-2*(i*i+j*j));
    if (roll < 0)
        roll += 2*DECIMAL_M_PI;

    return EulerAngles(ra, de, roll);
}

///////////////////////////////////
///////// ATTITUDE CLASS //////////
///////////////////////////////////

Quaternion Attitude::GetQuaternion() const {
    switch (type) {
        case QuaternionType:
            return quaternion;
        case DCMType:
            return DCMToQuaternion(dcm);
        default:
            throw std::runtime_error("No representation available");
    }
}

Mat3 Attitude::GetDCM() const {
    switch (type) {
        case DCMType:
            return dcm;
        case QuaternionType:
            return QuaternionToDCM(quaternion);
        default:
            throw std::runtime_error("No representation available");
    }
}

EulerAngles Attitude::ToSpherical() const {
    switch (type) {
        case DCMType:
            return GetQuaternion().ToSpherical();
        case QuaternionType:
            return quaternion.ToSpherical();
        default:
            throw std::runtime_error("No representation available");
    }
}

Vec3 Attitude::Rotate(const Vec3 &vec) const {
    switch (type) {
        case DCMType:
            return dcm*vec;
        case QuaternionType:
            return quaternion.Rotate(vec);
        default:
            throw std::runtime_error("No representation available");
    }
}

///////////////////////////////////
////// CONVERSION FUNCTIONS ///////
///////////////////////////////////

Mat3 QuaternionToDCM(const Quaternion &quat) {
    Vec3 x = quat.Rotate({1, 0, 0});
    Vec3 y = quat.Rotate({0, 1, 0});
    Vec3 z = quat.Rotate({0, 0, 1});
    return {
        x.x, y.x, z.x,
        x.y, y.y, z.y,
        x.z, y.z, z.z,
    };
}

Quaternion DCMToQuaternion(const Mat3 &dcm) {
    // Make a quaternion that rotates the reference frame X-axis into the dcm's X-axis, just like
    // the DCM itself does
    Vec3 oldXAxis = Vec3({1, 0, 0});
    Vec3 newXAxis = dcm.Column(0);  // this is where oldXAxis is mapped to
    assert(DECIMAL_ABS(newXAxis.Magnitude()-1) < DECIMAL(0.001));
    Vec3 xAlignAxis = oldXAxis.CrossProduct(newXAxis).Normalize();
    decimal xAlignAngle = AngleUnit(oldXAxis, newXAxis);
    Quaternion xAlign(xAlignAxis, xAlignAngle);

    // Make a quaternion that will rotate the Y-axis into place
    Vec3 oldYAxis = xAlign.Rotate({0, 1, 0});
    Vec3 newYAxis = dcm.Column(1);
    // we still need to take the cross product, because acos returns a value in [0,pi], and thus we
    // need to know which direction to rotate before we rotate. We do this by checking if the cross
    // product of old and new y axes is in the same direction as the new X axis.
    bool rotateClockwise = oldYAxis.CrossProduct(newYAxis) * newXAxis > 0;  // * is dot product
    Quaternion yAlign({1, 0, 0}, AngleUnit(oldYAxis, newYAxis) * (rotateClockwise ? 1 : -1));

    // We're done! There's no need to worry about the Z-axis because the handed-ness of the
    // coordinate system is always preserved, which means the Z-axis is uniquely determined as the
    // cross product of the X- and Y-axes. This goes to show that DCMs store redundant information.

    // we want the y alignment to have memory of X, which means we put its multiplication on the
    // right
    return xAlign*yAlign;
}

Quaternion SphericalToQuaternion(decimal ra, decimal dec, decimal roll) {
    assert(roll >= DECIMAL(0.0) && roll <= 2*DECIMAL_M_PI);
    assert(ra >= DECIMAL(0.0) && ra <= 2*DECIMAL_M_PI);
    assert(dec >= -DECIMAL_M_PI && dec <= DECIMAL_M_PI);

    // when we are modifying the coordinate axes, the quaternion multiplication works so that the
    // rotations are applied from left to right. This is the opposite as for modifying vectors.

    // It is indeed correct that a positive rotation in our right-handed coordinate frame is in the
    // clockwise direction when looking down/through the axis of rotation. Just like the right hand
    // rule for magnetic field around a current-carrying conductor.
    Quaternion a = Quaternion({ 0, 0, 1 }, ra);
    Quaternion b = Quaternion({ 0, 1, 0 }, -dec);
    Quaternion c = Quaternion({ 1, 0, 0 }, -roll);
    Quaternion result = a*b*c;
    assert(result.IsUnit(0.00001));
    return result;
}

Vec3 SphericalToSpatial(const decimal ra, const decimal de) {
    return {
        cos(ra)*cos(de),
        sin(ra)*cos(de),
        sin(de),
    };
}

void SpatialToSpherical(const Vec3 &vec, decimal &ra, decimal &de) {
    ra = atan2(vec.y, vec.x);
    if (ra < 0)
        ra += DECIMAL_M_PI*2;
    de = asin(vec.z);
}

}  // namespace found
