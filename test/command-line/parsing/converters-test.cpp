#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <getopt.h>

#include <string>

#include "test/common/common.hpp"

#include "src/common/spatial/attitude-utils.hpp"
#include "src/providers/converters.hpp"
#include "src/common/decimal.hpp"

namespace found {

TEST(ConvertersTest, TestEAComma) {
    std::string str = "0,45,90";
    EulerAngles angles = strtoea(str);

    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0), angles.ra);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(45), angles.de);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(90), angles.roll);
}

TEST(ConvertersTest, TestEASpace) {
    std::string str = "4.2 3.9 -9.5";
    EulerAngles angles = strtoea(str);

    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(4.2), angles.ra);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(3.9), angles.de);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(-9.5), angles.roll);
}

TEST(ConvertersTest, TestBoolFalse) {
    ASSERT_FALSE(strtobool(""));
    ASSERT_FALSE(strtobool("0"));
    ASSERT_FALSE(strtobool("false"));
}

TEST(ConvertersTest, TestBoolTrue) {
    ASSERT_TRUE(strtobool("1"));
    ASSERT_TRUE(strtobool("true"));
    ASSERT_TRUE(strtobool("not_false"));
}

TEST(ConvertersTest, TestDecimal) {
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0.9876), strtodecimal("0.9876"));
}

}  // namespace found
