#ifndef SRC_COMMON_TIME_TIME_HPP_
#define SRC_COMMON_TIME_TIME_HPP_

#include <cstdint>
#include <ctime>

#include "common/decimal.hpp"

namespace found {

// Average Delta UT1 in seconds, based on USNO data
#define AVG_DELTA_UT1 0.087497

/// Nanoseconds per second
constexpr double NS_PER_SEC = 1000000000ULL;
/// Average Delta UT1 in nanoseconds (pre-computed for efficiency)
constexpr double AVG_DELTA_UT1_NS = AVG_DELTA_UT1 * NS_PER_SEC;
/// Nanoseconds per day (86400 seconds)
constexpr double NS_PER_DAY = 86400.0 * NS_PER_SEC;
/// Seconds per day
constexpr double SEC_PER_DAY = 86400.0;
/// Minutes per day
constexpr double MIN_PER_DAY = 1440.0;
/// Hours per day
constexpr double HOURS_PER_DAY = 24.0;

/// Julian date at Unix epoch (January 1, 1970 00:00:00 UTC)
constexpr double JULIAN_UNIX_EPOCH = 2440587.5;
/// J2000.0 epoch Julian date (January 1, 2000 12:00:00 TT)
constexpr double J2000_JULIAN_DATE = 2451545.0;
/// Days per Julian century
constexpr double DAYS_PER_JULIAN_CENTURY = 36525.0;

/**
 * DateTime represents
 * a date and time
 */
struct DateTime {
    /// Nanoseconds since epoch
    uint64_t epochs;
    /// Year (e.g., 2023)
    int year;
    /// Month (1-12)
    int month;
    /// Day of the month (1-31)
    int day;
    /// Hour of the day (0-23)
    int hour;
    /// Minute of the hour (0-59)
    int minute;
    /// Second of the minute (0-60)
    int second;
    /// Nanosecond of the second (0-999999999)
    int nanosecond = 0;
};

/**
 * Obtains the current UTC time
 * in unix/epoch seconds
 * 
 * @return The current UTC Time
 */
DateTime getUTCTime();

/**
 * Obtains the current UT1 time
 * in unix/epoch seconds
 * 
 * @return The current UT1 time
 */
DateTime getUT1Time();

/**
 * Obtains the Julian date from
 * the given epochs in nanoseconds
 * 
 * @param epochs The epochs in nanoseconds
 * 
 * @return The Julian date corresponding
 * to the given epochs
 */
decimal getJulianDateTime(uint64_t epochs);

/**
 * Obtains the current Julian date
 * in decimal format
 * 
 * @param time The DateTime to convert
 * 
 * @return The Julian date corresponding
 * to time
 */
decimal getJulianDateTime(DateTime &time);

/**
 * Obtains the current Julian date
 * in decimal format
 * 
 * @return The current Julian date
 */
decimal getCurrentJulianDateTime();

/**
 * Obtains the Greenwich Mean Sidereal Time
 * in decimal format from the given epochs
 * 
 * @param epochs The epochs in nanoseconds
 * 
 * @return The Greenwich Mean Sidereal Time
 * corresponding to the given epochs in
 * degrees
 */
decimal getGreenwichMeanSiderealTime(uint64_t epochs);

/**
 * Obtains the Greenwich Mean Sidereal Time
 * in decimal format
 * 
 * @param time The DateTime to convert
 * 
 * @return The Greenwich Mean Sidereal Time
 * corresponding to time, in degrees
 * 
 * @pre The time must be in UT1
 */
decimal getGreenwichMeanSiderealTime(DateTime &time);

/**
 * Obtians the current Greenwich Mean Sidereal Time
 * in decimal format
 * 
 * @return The current Greenwich Mean Sidereal Time
 * in degrees
 */
decimal getCurrentGreenwichMeanSiderealTime();

}  // namespace found

#endif  // SRC_COMMON_TIME_TIME_HPP_
