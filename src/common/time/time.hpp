#ifndef SRC_COMMON_TIME_TIME_HPP_
#define SRC_COMMON_TIME_TIME_HPP_

#include <cstdint>
#include <ctime>

#include "common/decimal.hpp"

namespace found {

// Average Delta UT1 in seconds, based on USNO data
#define AVG_DELTA_UT1 0.087497

/// Nanoseconds per second
#define NS_PER_SEC 1000000000ULL
/// Average Delta UT1 in nanoseconds (pre-computed for efficiency)
#define AVG_DELTA_UT1_NS (static_cast<uint64_t>(AVG_DELTA_UT1 * NS_PER_SEC))
/// Nanoseconds per day (86400 seconds)
#define NS_PER_DAY (DECIMAL(86400.0) * NS_PER_SEC)
/// Seconds per day
#define SEC_PER_DAY DECIMAL(86400.0)
/// Minutes per day
#define MIN_PER_DAY DECIMAL(1440.0)
/// Hours per day
#define HOURS_PER_DAY DECIMAL(24.0)

/// Julian date at Unix epoch (January 1, 1970 00:00:00 UTC)
#define JULIAN_UNIX_EPOCH DECIMAL(2440587.5)
/// J2000.0 epoch Julian date (January 1, 2000 12:00:00 TT)
#define J2000_JULIAN_DATE DECIMAL(2451545.0)
/// Days per Julian century
#define DAYS_PER_JULIAN_CENTURY DECIMAL(36525.0)
/// Defines Feb 28, 1990 for the Julian date calculation
#define JD_CONSTANT_FEB_1990 DECIMAL(190002.5)

/**
 * DateTime represents
 * a date and time
 */
struct DateTime {
    /// Nanoseconds since epoch
    uint64_t epochs;
    /// Year (e.g., 2023)
    uint64_t year;
    /// Month (1-12)
    uint64_t month;
    /// Day of the month (1-31)
    uint64_t day;
    /// Hour of the day (0-23)
    uint64_t hour;
    /// Minute of the hour (0-59)
    uint64_t minute;
    /// Second of the minute (0-60)
    uint64_t second;
    /// Nanosecond of the second (0-999999999)
    uint64_t nanosecond = 0;
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
