#include "common/time/time.hpp"

#include <ctime>

#include "common/decimal.hpp"

// NOTE: We use a .cpp file instead of just putting these into
// the header file as inlined functions since these functions
// can get more complex and can be modified with different
// spacecraft requirements. For instance, the functions that
// obtain UTC and UT1 time can be modified to use an onboard
// atomic clock or GPS reciever.

// NOTE: To use approximations for getting the Julian Datetime
// and GMST for the current time, define the macro FAST
// #define FAST

namespace found {

DateTime getUTCTime() {
    std::time_t now = std::time(nullptr);
    std::tm* utc_time = std::gmtime(&now);

    return {
        now,
        static_cast<int>(utc_time->tm_year + 1900),  // tm_year is years since 1900
        static_cast<int>(utc_time->tm_mon + 1),      // tm_mon is months since January (0-11)
        static_cast<int>(utc_time->tm_mday),         // tm_mday is day of the month (1-31)
        static_cast<int>(utc_time->tm_hour),         // tm_hour is hours since midnight (0-23)
        static_cast<int>(utc_time->tm_min),          // tm_min is minutes after the hour (0-59)
        static_cast<int>(utc_time->tm_sec)           // tm_sec is seconds after the minute (0-60)
    };
}

DateTime getUT1Time() {
    // TODO: For simplicity, we return an approximation of UT1,
    // whose formula is UT1 = UTC + Delta UT1. This should be
    // replaced with either a table lookup or some curve function.
    //
    // Based on https://maia.usno.navy.mil/ser7/ser7.dat, which
    // provides Delta UT1 values from 2025 to 2026, the average is
    // 0.087497 seconds, so we use that as a rough approximation.
    DateTime now = getUTCTime();
    now.epochs += AVG_DELTA_UT1;  // Adjust for UT1, this is a rough approximation
    now.second += AVG_DELTA_UT1;  // Adjust seconds accordingly
    return now;
}

decimal getJulianDateTime(DateTime &time) {
    // Purportedly, the Julian date is also
    // time.epochs / 86400.0 + 2440587.5, but
    // this is apparently just an approximation.

    // The below formula is from https://aa.usno.navy.mil/faq/JD_formula
    int64_t A = 367 * time.year;
    int64_t B = (7 * (time.year + (time.month + 9) / 12)) / 4;
    int64_t C = (275 * time.month) / 9;
    decimal D = time.day + 1721013.5;
    decimal julianDate = A - B + C + D;

    return julianDate + time.hour / DECIMAL(24.0) +
           time.minute / DECIMAL(1440.0) + time.second / DECIMAL(86400.0) -
           DECIMAL(0.5) * (100 * time.year + time.month > 190002.5 ? 1 : -1) +
           DECIMAL(0.5);
}

decimal getCurrentJulianDateTime() {
    #ifdef FAST
        // If FAST is defined, we use the approximation
        return getJulianDateTime(std::time(nullptr));
    #else
        DateTime time = getUTCTime();
        return getJulianDateTime(time);
    #endif
}

decimal getJulianDateTime(std::time_t epochs) {
    return epochs / 86400.0 + 2440587.5;
}

decimal getGreenwichMeanSiderealTime(DateTime &time) {
    // Purportedly, the Greenwich Mean Sidereal Time (GMST)
    // is also 15(18.697374558 + 24.06570982441908 * (JDT - 2451545.0))
    // in degrees

    // The below formula is from http://tiny.cc/4wal001
    decimal JDT = getJulianDateTime(time);
    decimal D_tt = JDT - DECIMAL(2451545.0);  // Julian date - J2000.0
    decimal t = D_tt / DECIMAL(36525.0);  // Julian centuries since J2000.0

    return DECIMAL(280.46061837) +
           DECIMAL(360.98564736629) * D_tt +
           (DECIMAL(0.000387933) * t * t) -
           (t * t * t / DECIMAL(38710000.0));
}

decimal getCurrentGreenwichMeanSiderealTime() {
    #ifdef FAST
        // If FAST is defined, we use the approximation
        return getGreenwichMeanSiderealTime(std::time(nullptr));
    #else
        DateTime time = getUTCTime();
        return getGreenwichMeanSiderealTime(time);
    #endif
}

decimal getGreenwichMeanSiderealTime(std::time_t epochs) {
    return 15 * (DECIMAL(18.697374558) + DECIMAL(24.06570982441908) * (getJulianDateTime(epochs) - DECIMAL(2451545.0)));
}

}  // namespace found
