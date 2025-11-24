
#include "common/spatial/attitude-utils.hpp"

#include <math.h>
#include <assert.h>

#include <stdexcept>

#include "common/logging.hpp"

#define DEFAULT_TOLERANCE DECIMAL(1e-3)

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

decimal Vec3::At(int i) const{
    if (i == 0) return x;
    if (i == 1) return y;
    if (i == 2) return z;
    throw std::runtime_error("index out of bounds");
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

bool Mat3::operator==(const Mat3 &other) const {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++){
            if (abs(other.At(i, j) - At(i, j)) < DEFAULT_TOLERANCE) return false;
        }
    }
    return true;
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
    decimal determinant = Det();
    if (determinant == 0) throw std::invalid_argument("Matrix is non-invertible");
    decimal scalar = 1 / Det();

    Mat3 res = {
        At(1,1)*At(2,2) - At(1,2)*At(2,1), At(0,2)*At(2,1) - At(0,1)*At(2,2), At(0,1)*At(1,2) - At(0,2)*At(1,1),
        At(1,2)*At(2,0) - At(1,0)*At(2,2), At(0,0)*At(2,2) - At(0,2)*At(2,0), At(0,2)*At(1,0) - At(0,0)*At(1,2),
        At(1,0)*At(2,1) - At(1,1)*At(2,0), At(0,1)*At(2,0) - At(0,0)*At(2,1), At(0,0)*At(1,1) - At(0,1)*At(1,0)
    };

    return res * scalar;
}

Mat3 Mat3::Cofactor() const {
    Mat3 C ={
        (At(1,1) * At(2,2) - At(1,2) * At(2,1)), (At(1,0) * At(2,2) - At(1,2) * At(2,0)), (At(1,0) * At(2,1) - At(1,1) * At(2,0)),
        (At(0,1) * At(2,2) - At(0,2) * At(2,1)), (At(0,0) * At(2,2) - At(0,2) * At(2,0)), (At(0,0) * At(2,1) - At(0,1) * At(2,0)),
        (At(0,1) * At(1,2) - At(0,2) * At(1,1)), (At(0,0) * At(1,2) - At(0,2) * At(1,0)), (At(0,0) * At(1,1) - At(0,1) * At(1,0))
    };    

    return C;
}

Mat3 Mat3::Adjugate() const {
    return Cofactor().Transpose();
}

// Given a pair (u, v), solves (cos, sin) * (-v, u) = 0 for cos and sin
// Check the eigenvalue paper (Eq. 3) for derivation
void GetCosSin(decimal u, decimal v, decimal& cos, decimal& sin){
    decimal length = sqrt(u*u + v*v);
    if (length != 0){
        cos = u/length;
        sin = v/length;
        if (cos > 0){
            cos = -cos;
            sin = -sin;
        }
    }
    else{
        cos = -1;
        sin = 0;
    }
}

// Checks if the result of the iteration has converged to an acceptable degree
bool Converged(decimal diag1, decimal diag2, decimal superdiag){
    decimal sum = DECIMAL_ABS(diag1) + DECIMAL_ABS(diag2);
    return sum + superdiag == sum;
}

// takes in a vec3 of eigenvalues and returns a sorted vec3
Vec3 sortEigenvalues(Vec3 eigenvalues){
    decimal x = eigenvalues.x;
    decimal y = eigenvalues.y;
    decimal z = eigenvalues.z;
    if (x > y){
        if (y > z) return {0, 1, 2};
        if (x > z) return {0, 2, 1};
        return {2, 0, 1};
    }
    if (x > z) return {1, 0, 2};
    if (y > z) return {1, 2, 0};
    return {2, 1, 0};
}

// Takes in a mat3 of eigenvectors and sorts them according to eigenvalue
//
// This WILL break if an eigenvalue is degenerate, so just make sure our satellite doesn't do that 👍
// Since we're dealing with really precise floats this *should* be very improbably but who knows
// If our satellite explodes randomly I'm sorry
// I will probably fix this later maybe - Senuka
Mat3 sortEigenvectors(Vec3 sortedEigenvalues, Mat3 eigenvectors, Mat3 matrix){
    Vec3 eigenvector0;
    Vec3 eigenvector1;
    Vec3 eigenvector2;

    for (int i = 0; i < 3; i ++){
        if (abs(eigenvectors.At(i, 0)) < DEFAULT_TOLERANCE) continue;
        decimal eigenvalue0 = (matrix.Row(i) * eigenvectors.Column(0))/eigenvectors.At(i, 0);
        if (abs(eigenvalue0 - sortedEigenvalues.x) < DEFAULT_TOLERANCE) eigenvector0 = eigenvectors.Column(0);
        if (abs(eigenvalue0 - sortedEigenvalues.y) < DEFAULT_TOLERANCE) eigenvector1 = eigenvectors.Column(0);
        if (abs(eigenvalue0 - sortedEigenvalues.z) < DEFAULT_TOLERANCE) eigenvector2 = eigenvectors.Column(0);
    }

    for (int i = 0; i < 3; i ++){
        if (abs(eigenvectors.At(i, 1)) < DEFAULT_TOLERANCE) continue;
        decimal eigenvalue1 = (matrix.Row(i) * eigenvectors.Column(1))/eigenvectors.At(i, 1);
        if (abs(eigenvalue1 - sortedEigenvalues.x) < DEFAULT_TOLERANCE) eigenvector0 = eigenvectors.Column(1);
        if (abs(eigenvalue1 - sortedEigenvalues.y) < DEFAULT_TOLERANCE) eigenvector1 = eigenvectors.Column(1);
        if (abs(eigenvalue1 - sortedEigenvalues.z) < DEFAULT_TOLERANCE) eigenvector2 = eigenvectors.Column(1);
    }

    for (int i = 0; i < 3; i ++){
        if (abs(eigenvectors.At(i, 2)) < DEFAULT_TOLERANCE) continue;
        decimal eigenvalue2 = (matrix.Row(i) * eigenvectors.Column(2))/eigenvectors.At(i, 2);
        if (abs(eigenvalue2 - sortedEigenvalues.x) < DEFAULT_TOLERANCE) eigenvector0 = eigenvectors.Column(2);
        if (abs(eigenvalue2 - sortedEigenvalues.y) < DEFAULT_TOLERANCE) eigenvector1 = eigenvectors.Column(2);
        if (abs(eigenvalue2 - sortedEigenvalues.z) < DEFAULT_TOLERANCE) eigenvector2 = eigenvectors.Column(2);
    }

    return Mat3FromCols(eigenvector0, eigenvector1, eigenvector2);

}

/* Sorry, this function is gonna be long while I work on it (I'll clean it up later) - Senuka
*  The paper writes all the matrix multiplication out explicitly; while this might be technically faster since it lets us 
*  skip computing a couple positions, I think readability is more important here
* 
*/
Vec3 Mat3::EigenvaluesSymmetric() {
    if (_eigenvectorsAlreadyCalculated) return _eigenvalues;
    decimal half = static_cast<decimal>(0.5);

    // First compute Householder reflection to set b02 to 0
    // Eq. 2
    decimal c, s;
    GetCosSin(At(1,2), -At(0,2), c, s);
    Mat3 HouseholderRefl = {c,  s, 0, 
                            s, -c, 0,
                            0,  0, 1};
    Mat3 B = HouseholderRefl * (*this) * HouseholderRefl; //technically should be transpose on the left but it's the same matrix
    // Matrix B is now a tridiagonal matrix

    // This matrix will be the product of all the reflections, and will eventually turn into our list of eigenvectors
    Mat3 ReflProduct = {c,s,0,
                        s,-c,0,
                        0,0,1};
    
    // The smallest number we can represent is 2^-alpha          
    int alpha = 2; // TODO Figure out what this is for decimal; for some reason only 2 seems to work??? I have no idea why
    int i = 0, imax = 0, power = 0;
    decimal c2, s2;

    if (DECIMAL_ABS(B.At(1,2)) <= DECIMAL_ABS(B.At(0,1))){
        // Eq. 12
        // finds alpha in b12 = x * 2^alpha
        std::frexp(B.At(1,2), &power);
        imax = 2 * (power + alpha + 1);
        for (i = 0; i < imax; ++i){
            // Compute Givens reflection of B in Eq. 4
            GetCosSin(half * (B.At(0,0) - B.At(1,1)), B.At(0,1), c2, s2); 
            s = DECIMAL_SQRT(half * (1 - c2));
            c = s2 / (2 * s);
            Mat3 GivensReflection = {c, 0, -s,
                                     s, 0, c,
                                     0, 1, 0};

            // Update B
            B = GivensReflection.Transpose() * B * GivensReflection;
            // Update ReflProduct
            ReflProduct = ReflProduct * GivensReflection;

            if (Converged(B.At(0,0), B.At(1,1), B.At(0,1))){
                GetCosSin(half * (B.At(0,0) - B.At(1,1)), B.At(0,1), c2, s2);
                s = DECIMAL_SQRT(half * (1 - c2));
                c = half * s2 / s;
                HouseholderRefl = { c,  s, 0, 
                                    s, -c, 0,
                                    0,  0, 1};

                // This matrix is now the diagonal estimate
                B = HouseholderRefl * (*this) * HouseholderRefl;
                // This matrix is now the eigenvector matrix estimate
                ReflProduct = ReflProduct * HouseholderRefl;
                break;
            }
        }
    }
    else{
        // Eq. 12
        // finds alpha in b01 = x * 2^alpha
        std::frexp(B.At(0,1), &power);
        imax = 2 * (power + alpha + 1);
        for (i = 0; i < imax; ++i){
            // Compute Givens reflection of B in Eq. 4
            GetCosSin(half * (B.At(1,1) - B.At(2,2)), B.At(1,2), c2, s2);
            s = DECIMAL_SQRT(half * (1 - c2));
            c = s2 / (2 * s);
            Mat3 GivensReflection = {0, 1, 0,
                                     c, 0, -s,
                                     s, 0, c};

            // Update B
            B = GivensReflection.Transpose() * B * GivensReflection;
            // Update ReflProduct
            ReflProduct = ReflProduct * GivensReflection;

            if (Converged(B.At(1,1), B.At(2,2), B.At(1,2))){
                GetCosSin(half * (B.At(0,0) - B.At(1,1)), B.At(0,1), c2, s2);
                s = DECIMAL_SQRT(half * (1 - c2));
                c = half * s2 / s;
                HouseholderRefl = { c,  s, 0, 
                                    s, -c, 0,
                                    0,  0, 1};

                // This matrix is now the diagonal estimate
                B = HouseholderRefl * (*this) * HouseholderRefl;
                // This matrix is now the eigenvector matrix estimate
                ReflProduct = ReflProduct * HouseholderRefl;
                break;
            }
        }
    }

    // Get the eigenvalues and eigenvectors, sort them, and store them
    Vec3 unsortedEigenvalues = {B.At(0,0), B.At(1,1), B.At(2,2)};
    Vec3 eigenvalueOrder = sortEigenvalues(unsortedEigenvalues);
    _eigenvalues = {unsortedEigenvalues.At(eigenvalueOrder.x), unsortedEigenvalues.At(eigenvalueOrder.y), unsortedEigenvalues.At(eigenvalueOrder.z)};
    Mat3 sortedEigenvectors = sortEigenvectors(_eigenvalues, ReflProduct, *this);
    _eigenvector1 = sortedEigenvectors.Column(0);
    _eigenvector2 = sortedEigenvectors.Column(1);
    _eigenvector3 = sortedEigenvectors.Column(2);
    _eigenvectorsAlreadyCalculated = true;
    return _eigenvalues;
}

Mat3 Mat3::EigenvectorsSymmetric() {
    
    if (!_eigenvectorsAlreadyCalculated) EigenvaluesSymmetric();
    Mat3 eigenvectors = Mat3FromCols(_eigenvector1, _eigenvector2, _eigenvector3);
    return eigenvectors;
}


/// 3x3 identity matrix
const Mat3 kIdentityMat3 = {1,0,0,
                            0,1,0,
                            0,0,1};


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
    // and 2, we store the conjugate of the quaternion (double check why?), which means we need to
    // invert the final de and roll terms, as well as negate all the terms involving a mix between
    // the real and imaginary parts.
    decimal ra = atan2(2*(-real*k+i*j), 1-2*(j*j+k*k));
    if (ra < 0)
        ra += 2*DECIMAL_M_PI;
    decimal de = -asin(2*(-real*j-i*k));  // allow de to be positive or negaive, as is convention
    decimal roll = -atan2(2*(-real*i+j*k), 1-2*(i*i+j*j));
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

Mat3 Mat3FromCols(Vec3 col1, Vec3 col2, Vec3 col3){
    Mat3 output = { col1.x, col2.x, col3.x,
                    col1.y, col2.y, col3.y,
                    col1.z, col2.z, col3.z};
    return output;
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
    Quaternion result = (a*b*c).Conjugate();
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
