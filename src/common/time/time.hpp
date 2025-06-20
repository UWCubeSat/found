#ifndef TIME_H
#define TIME_H

#include <ctime>

#include "common/decimal.hpp"

namespace found {

// Average Delta UT1 in seconds, based on USNO data
#define AVG_DELTA_UT1 0.087497

/**
 * DateTime represents
 * a date and time
 */
struct DateTime {
    /// Seconds since epoch
    std::time_t epochs;
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
 * the given epochs in seconds
 * 
 * @param epochs The epochs in seconds
 * 
 * @return The Julian date corresponding
 * to the given epochs
 */
decimal getJulianDateTime(std::time_t epochs);

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
 * @param epochs The epochs in seconds
 * 
 * @return The Greenwich Mean Sidereal Time
 * corresponding to the given epochs in
 * degrees
 */
decimal getGreenwichMeanSiderealTime(std::time_t epochs);

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

#endif  // TIME_H
