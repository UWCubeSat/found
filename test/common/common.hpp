#ifndef COMMON_H
#define COMMON_H

#include <gtest/gtest.h>

#include "src/style/decimal.hpp"

#define DEFAULT_TOLERANCE 1e-3

#define ASSERT_DECIMAL_EQ(val1, val2, tolerance) ASSERT_LT(DECIMAL_ABS(val1 - val2), tolerance)

#define ASSERT_DECIMAL_EQ_DEFAULT(val1, val2) ASSERT_DECIMAL_EQ(val1, val2, DEFAULT_TOLERANCE)

#endif  // COMMON_H
