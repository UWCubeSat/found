#ifndef DECIMAL_H
#define DECIMAL_H

#include <cmath>
#include <string>
#include <limits>

// Use FOUND_FLOAT_MODE to distinguish
// the real type of float, and to change
// code accordingly.
#ifdef FOUND_FLOAT_MODE
    typedef float decimal;
    #define STR_TO_DECIMAL(x) std::stof(x)
#else
    typedef double decimal;
    #define STR_TO_DECIMAL(x) std::stod(x)
#endif

// This should only be used sparingly.
// It's better to verbosely typecast sometimes. Only use these to prevent promotions.
// The reason why this isn't used everywhere instead of the wrapped macros is
// because the code becomes hard to read when there are multiple layers of typecasting.
// With this method, we might have more preprocessing to do BUT the code remains readable
// as the methods remain relatively the same.
#define DECIMAL(x) (static_cast<decimal>(x))

// Math Constants wrapped with Decimal typecast
#define DECIMAL_M_E             (DECIMAL(M_E))           /* e */
#define DECIMAL_M_LOG2E         (DECIMAL(M_LOG2E))       /* log_2 e */
#define DECIMAL_M_LOG10E        (DECIMAL(M_LOG10E))      /* log_10 e */
#define DECIMAL_M_LN2           (DECIMAL(M_LN2))         /* log_e 2 */
#define DECIMAL_M_LN10          (DECIMAL(M_LN10))        /* log_e 10 */
#define DECIMAL_M_PI            (DECIMAL(M_PI))          /* pi */
#define DECIMAL_M_PI_2          (DECIMAL(M_PI_2))        /* pi/2 */
#define DECIMAL_M_PI_4          (DECIMAL(M_PI_4))        /* pi/4 */
#define DECIMAL_M_1_PI          (DECIMAL(M_1_PI))        /* 1/pi */
#define DECIMAL_M_2_PI          (DECIMAL(M_2_PI))        /* 2/pi */
#define DECIMAL_M_2_SQRTPI      (DECIMAL(M_2_SQRTPI))    /* 2/sqrt(pi) */
#define DECIMAL_M_SQRT2         (DECIMAL(M_SQRT2))       /* sqrt(2) */
#define DECIMAL_M_SQRT1_2       (DECIMAL(M_SQRT1_2))     /* 1/sqrt(2) */
#define DECIMAL_M_R_E           (DECIMAL(6371008.7714))  /* Earth's Radius */
#define DECIMAL_INF             (DECIMAL(INFINITY))      /* Infinity */

// Math Functions wrapped with Decimal typecast
#define DECIMAL_POW(base,power) (DECIMAL(std::pow(base), power))
#define DECIMAL_SQRT(x)         (DECIMAL(std::sqrt(x)))
#define DECIMAL_LOG(x)          (DECIMAL(std::log(x)))
#define DECIMAL_EXP(x)          (DECIMAL(std::exp(x)))
#define DECIMAL_ERF(x)          (DECIMAL(std::erf(x)))

// Rouding methods wrapped with Decimal typecast)
#define DECIMAL_ROUND(x)        (DECIMAL(std::round(x)))
#define DECIMAL_CEIL(x)         (DECIMAL(std::ceil(x)))
#define DECIMAL_FLOOR(x)        (DECIMAL(std::floor(x)))
#define DECIMAL_ABS(x)          (DECIMAL(std::abs(x)))

// Trig Methods wrapped with Decimal typecast)
#define DECIMAL_SIN(x)          (DECIMAL(std::sin(x)))
#define DECIMAL_COS(x)          (DECIMAL(std::cos(x)))
#define DECIMAL_TAN(x)          (DECIMAL(std::tan(x)))
#define DECIMAL_ASIN(x)         (DECIMAL(std::asin(x)))
#define DECIMAL_ACOS(x)         (DECIMAL(std::acos(x)))
#define DECIMAL_ATAN(x)         (DECIMAL(std::atan(x)))
#define DECIMAL_ATAN2(x,y)      (DECIMAL(std::atan2(x,y)))

// Float methods wrapped with Decimal typecast)
#define DECIMAL_FMA(x,y,z)      (DECIMAL(std::fma(x),y,z))
#define DECIMAL_HYPOT(x,y)      (DECIMAL(std::hypot(x),y))

#endif  // DECIMAL_H
