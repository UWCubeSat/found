#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>

#include "src/common/logging.hpp"

namespace found {

TEST(LoggingTest, TestLogInfo) {
    testing::internal::CaptureStdout();

    LOG(INFO, "Meep");

    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_THAT(
        output,
        testing::MatchesRegex(
            "\\[INFO\\s[0-9]{4}-[0-9]{2}-[0-9]{2}\\s[0-9]{2}:[0-9]{2}:[0-9]{2}\\s[A-Z]+\\] Meep\\s*"));
}

TEST(LoggingTest, TestLogWarning) {
    testing::internal::CaptureStderr();

    LOG(WARN, "Meep Warning");

    std::string output = testing::internal::GetCapturedStderr();

    ASSERT_THAT(
        output,
        testing::MatchesRegex(
            "\\[WARN\\s[0-9]{4}-[0-9]{2}-[0-9]{2}\\s[0-9]{2}:[0-9]{2}:[0-9]{2}\\s[A-Z]+\\] Meep Warning\\s*"));
}

TEST(LoggingTest, TestLogError) {
    testing::internal::CaptureStderr();

    LOG(ERROR, "Meep Error");

    std::string output = testing::internal::GetCapturedStderr();

    ASSERT_THAT(output,
        testing::MatchesRegex(
            "\\[ERROR\\s[0-9]{4}-[0-9]{2}-[0-9]{2}\\s[0-9]{2}:[0-9]{2}:[0-9]{2}\\s[A-Z]+\\] Meep Error\\s*"));
}


}  // namespace found
