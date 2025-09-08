#include <gtest/gtest.h>

#include <chrono>
#include <ctime>
#include <thread>

#include "test/common/common.hpp"

#include "src/common/time/time.hpp"

namespace found {

#define SLEEP_TIME 100

#define SECONDS_TOLERANCE 3
#define MINUTES_TOLERANCE 1
#define HOURS_TOLERANCE 1

TEST(TimeTest, TestGetUTCTime) {
    // Get current time point from system_clock
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));  // Ensure we have a different time point
    DateTime actual = getUTCTime();

    // Duration since epoch in seconds
    std::chrono::seconds epoch_seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());

    // Convert to time_t (epoch seconds)
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    // Convert to UTC calendar time
    std::tm* expected = std::gmtime(&now_c);

    ASSERT_RANGE(actual.epochs,
                 static_cast<int>(epoch_seconds.count()),
                 static_cast<int>(epoch_seconds.count() + 5));
    // NOTE: In the rare case you run this at midnight on new years UTC, this may fail
    ASSERT_EQ(static_cast<int>(expected->tm_year + 1900), actual.year);
    // NOTE: In the rare case you run this when the month changes in UTC, this may fail
    ASSERT_EQ(static_cast<int>(expected->tm_mon + 1), actual.month);
    if (expected->tm_hour == 23 &&
            (expected->tm_min > 60 - MINUTES_TOLERANCE && expected->tm_min < MINUTES_TOLERANCE) &&
            (expected->tm_sec > 60 - SECONDS_TOLERANCE && expected->tm_sec < SECONDS_TOLERANCE)) {
        ASSERT_RANGE(actual.day,
                     static_cast<int>(expected->tm_mday),
                     static_cast<int>(expected->tm_mday + 1));
    } else {
        ASSERT_EQ(static_cast<int>(expected->tm_mday), actual.day);
    }

    // Check the hour with tolerance if warranted
    if ((expected->tm_min > 60 - MINUTES_TOLERANCE && expected->tm_min < MINUTES_TOLERANCE) &&
        (expected->tm_sec > 60 - SECONDS_TOLERANCE && expected->tm_sec < SECONDS_TOLERANCE)) {
        ASSERT_RANGE(actual.hour,
                     static_cast<int>(expected->tm_hour),
                     static_cast<int>(expected->tm_hour + HOURS_TOLERANCE));
    } else {
        ASSERT_EQ(static_cast<int>(expected->tm_hour), actual.hour);
    }

    // Check the minute with tolerance if warranted
    if (expected->tm_sec > 60 - SECONDS_TOLERANCE && expected->tm_sec < SECONDS_TOLERANCE) {
        ASSERT_RANGE(actual.minute,
                     static_cast<int>(expected->tm_min),
                     static_cast<int>(expected->tm_min + MINUTES_TOLERANCE));
    } else {
        ASSERT_EQ(static_cast<int>(expected->tm_min), actual.minute);
    }

    // Check the second with tolerance
    ASSERT_RANGE(actual.second,
                 static_cast<int>(expected->tm_sec),
                 static_cast<int>(expected->tm_sec + SECONDS_TOLERANCE));
}

TEST(TimeTest, TestGetUT1Time) {
    // Get current time point from system_clock
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));  // Ensure we have a different time point
    DateTime actual = getUT1Time();

    // Duration since epoch in seconds
    std::chrono::seconds epoch_seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());

    // Convert to time_t (epoch seconds)
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    // Convert to UTC calendar time
    std::tm* expected = std::gmtime(&now_c);

    ASSERT_RANGE(actual.epochs,
                 static_cast<int>(epoch_seconds.count() + AVG_DELTA_UT1),
                 static_cast<int>(epoch_seconds.count() + + AVG_DELTA_UT1 + 5));
    // NOTE: In the rare case you run this at midnight on new years UTC, this may fail
    ASSERT_EQ(static_cast<int>(expected->tm_year + 1900), actual.year);
    // NOTE: In the rare case you run this when the month changes in UTC, this may fail
    ASSERT_EQ(static_cast<int>(expected->tm_mon + 1), actual.month);
    if (expected->tm_hour == 23 &&
            (expected->tm_min > 60 - MINUTES_TOLERANCE && expected->tm_min < MINUTES_TOLERANCE) &&
            (expected->tm_sec > 60 - SECONDS_TOLERANCE && expected->tm_sec < SECONDS_TOLERANCE)) {
        ASSERT_RANGE(actual.day,
                     static_cast<int>(expected->tm_mday),
                     static_cast<int>(expected->tm_mday + 1));
    } else {
        ASSERT_EQ(static_cast<int>(expected->tm_mday), actual.day);
    }

    // Check the hour with tolerance if warranted
    if ((expected->tm_min > 60 - MINUTES_TOLERANCE && expected->tm_min < MINUTES_TOLERANCE) &&
        (expected->tm_sec > 60 - SECONDS_TOLERANCE && expected->tm_sec < SECONDS_TOLERANCE)) {
        ASSERT_RANGE(actual.hour,
                     static_cast<int>(expected->tm_hour),
                     static_cast<int>(expected->tm_hour + HOURS_TOLERANCE));
    } else {
        ASSERT_EQ(static_cast<int>(expected->tm_hour), actual.hour);
    }

    // Check the minute with tolerance if warranted
    if (expected->tm_sec > 60 - SECONDS_TOLERANCE && expected->tm_sec < SECONDS_TOLERANCE) {
        ASSERT_RANGE(actual.minute,
                     static_cast<int>(expected->tm_min),
                     static_cast<int>(expected->tm_min + MINUTES_TOLERANCE));
    } else {
        ASSERT_EQ(static_cast<int>(expected->tm_min), actual.minute);
    }

    // Check the second with tolerance
    ASSERT_RANGE(actual.second,
                 static_cast<int>(expected->tm_sec + AVG_DELTA_UT1),
                 static_cast<int>(expected->tm_sec + + AVG_DELTA_UT1 + SECONDS_TOLERANCE));
}

TEST(TimeTest, TestGetJulianDateNow) {
    DateTime time = getUT1Time();
    #ifdef FOUND_FLOAT_MODE
        sleep(1.5);
    #endif
    decimal julianDate = getCurrentJulianDateTime();
    decimal expectedJulianDate = time.epochs / 86400.0 + 2440587.5;

    #ifndef FOUND_FLOAT_MODE
        ASSERT_RANGE(julianDate, expectedJulianDate, expectedJulianDate + SECONDS_TOLERANCE);
    #else
        ASSERT_RANGE(julianDate, expectedJulianDate - SECONDS_TOLERANCE, expectedJulianDate + SECONDS_TOLERANCE);
    #endif
}

TEST(TimeTest, TestGetJulianDateBefore1900) {
    // Start of the American Civil War
    DateTime time{
        -3430878211,
        1861,
        4,
        12,
        18,
        16,
        29
    };

    decimal julianDate = getJulianDateTime(time);
    decimal expectedJulianDate = time.epochs / 86400.0 + 2440587.5;

    ASSERT_DECIMAL_EQ_DEFAULT(expectedJulianDate, julianDate);
}

TEST(TimeTest, TestGetJulianDate) {
    // The day/time my school closed due to COVID-19
    DateTime time{
        1584101520,
        2020,
        3,
        13,
        12,
        12,
        0
    };

    decimal julianDate = getJulianDateTime(time);
    decimal expectedJulianDate = time.epochs / 86400.0 + 2440587.5;

    ASSERT_DECIMAL_EQ_DEFAULT(expectedJulianDate, julianDate);
}

TEST(TimeTest, TestGetJulianDateTimeEpoch) {
    // Start of the American Civil War
    DateTime time{
        -3430878211,
        1861,
        4,
        12,
        18,
        16,
        29
    };
    decimal julianDate = getJulianDateTime(time.epochs);
    decimal expectedJulianDate = getJulianDateTime(time);

    ASSERT_DECIMAL_EQ_DEFAULT(expectedJulianDate, julianDate);
}

TEST(TimeTest, TestGetGreenwichMeanSiderealTimeNow) {
    DateTime time = getUT1Time();
    #ifdef FOUND_FLOAT_MODE
        sleep(1.5);
    #endif
    decimal gmst = getCurrentGreenwichMeanSiderealTime();
    decimal expectedGmst = 15 * (DECIMAL(18.697374558) + DECIMAL(24.06570982441908) *
            (getJulianDateTime(time) - DECIMAL(2451545.0)));

    #ifdef FOUND_FLOAT_MODE
        ASSERT_RANGE(gmst, expectedGmst - SECONDS_TOLERANCE, expectedGmst + SECONDS_TOLERANCE);
    #else
        ASSERT_RANGE(gmst, expectedGmst, expectedGmst + SECONDS_TOLERANCE);
    #endif
}

TEST(TimeTest, TestGetGreenwichMeanSiderealTime) {
    // The day/time my school closed due to COVID-19
    DateTime time{
        1584101520,
        2020,
        3,
        13,
        12,
        12,
        0
    };

    decimal gmst = getGreenwichMeanSiderealTime(time);
    decimal expectedGmst = 15 * (DECIMAL(18.697374558) + DECIMAL(24.06570982441908) *
            (getJulianDateTime(time) - DECIMAL(2451545.0)));

    ASSERT_DECIMAL_EQ_DEFAULT(expectedGmst, gmst);
}

TEST(TimeTest, TestGetGreenwichMeanSiderealTimeEpoch) {
    // The new millenium
    DateTime time{
        946598400,
        2000,
        1,
        0,
        0,
        0,
        0
    };
    decimal gmst = getGreenwichMeanSiderealTime(time.epochs);
    decimal expectedGmst = getGreenwichMeanSiderealTime(time);

    ASSERT_DECIMAL_EQ_DEFAULT(expectedGmst, gmst);
}

}  // namespace found
