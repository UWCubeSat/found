#include "common/time/time.hpp"

#include <chrono>
#include <cstdint>
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
    // Use chrono for nanosecond precision
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::system_clock::duration duration = now.time_since_epoch();
    std::chrono::nanoseconds nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
    uint64_t epochs_ns = static_cast<uint64_t>(nanos.count());

    // Get broken-down time for year/month/day/etc.
    std::time_t now_t = std::chrono::system_clock::to_time_t(now);
    std::tm* utc_time = std::gmtime(&now_t);

    // Calculate nanoseconds within the current second
    std::chrono::seconds secs = std::chrono::duration_cast<std::chrono::seconds>(duration);
    uint64_t nanosecond = static_cast<uint64_t>((nanos -
            std::chrono::duration_cast<std::chrono::nanoseconds>(secs)).count());

    return {
        epochs_ns,
        static_cast<uint64_t>(utc_time->tm_year + 1900),  // tm_year is years since 1900
        static_cast<uint64_t>(utc_time->tm_mon + 1),      // tm_mon is months since January (0-11)
        static_cast<uint64_t>(utc_time->tm_mday),         // tm_mday is day of the month (1-31)
        static_cast<uint64_t>(utc_time->tm_hour),         // tm_hour is hours since midnight (0-23)
        static_cast<uint64_t>(utc_time->tm_min),          // tm_min is minutes after the hour (0-59)
        static_cast<uint64_t>(utc_time->tm_sec),          // tm_sec is seconds after the minute (0-60)
        nanosecond                                   // nanoseconds within current second
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
    // Add AVG_DELTA_UT1 in nanoseconds
    now.epochs += AVG_DELTA_UT1_NS;
    now.nanosecond += static_cast<uint64_t>((AVG_DELTA_UT1 - static_cast<uint64_t>(AVG_DELTA_UT1)) * NS_PER_SEC);
    return now;
}

decimal getJulianDateTime(DateTime &time) {
    // Purportedly, the Julian date is also
    // time.epochs / NS_PER_DAY + JULIAN_UNIX_EPOCH, but
    // this is apparently just an approximation.

    // The below formula is from https://aa.usno.navy.mil/faq/JD_formula
    int64_t A = 367 * time.year;
    int64_t B = (7 * (time.year + (time.month + 9) / 12)) / 4;
    int64_t C = (275 * time.month) / 9;
    decimal D = time.day + 1721013.5;
    decimal julianDate = A - B + C + D;

    return julianDate + time.hour / HOURS_PER_DAY +
           time.minute / MIN_PER_DAY + time.second / SEC_PER_DAY +
           time.nanosecond / NS_PER_DAY -
           DECIMAL(0.5) * (100 * time.year + time.month > JD_CONSTANT_FEB_1990 ? 1 : -1) +
           DECIMAL(0.5);
}

decimal getCurrentJulianDateTime() {
    #ifdef FAST
        // If FAST is defined, we use the approximation with nanosecond precision
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::chrono::nanoseconds nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch());
        return getJulianDateTime(static_cast<uint64_t>(nanos.count()));
    #else
        DateTime time = getUT1Time();
        return getJulianDateTime(time);
    #endif
}

decimal getJulianDateTime(uint64_t epochs) {
    // epochs is in nanoseconds, divide by nanoseconds per day
    return epochs / NS_PER_DAY + JULIAN_UNIX_EPOCH;
}

decimal getGreenwichMeanSiderealTime(DateTime &time) {
    // Purportedly, the Greenwich Mean Sidereal Time (GMST)
    // is also 15(18.697374558 + 24.06570982441908 * (JDT - J2000_JULIAN_DATE))
    // in degrees

    // The below formula is from http://tiny.cc/4wal001
    decimal JDT = getJulianDateTime(time);
    decimal D_tt = JDT - J2000_JULIAN_DATE;  // Julian date - J2000.0
    decimal t = D_tt / DAYS_PER_JULIAN_CENTURY;  // Julian centuries since J2000.0

    return DECIMAL(280.46061837) +
           DECIMAL(360.98564736629) * D_tt +
           (DECIMAL(0.000387933) * t * t) -
           (t * t * t / DECIMAL(38710000.0));
}

decimal getCurrentGreenwichMeanSiderealTime() {
    #ifdef FAST
        // If FAST is defined, we use the approximation with nanosecond precision
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::chrono::nanoseconds nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch());
        return getGreenwichMeanSiderealTime(static_cast<uint64_t>(nanos.count()));
    #else
        DateTime time = getUT1Time();
        return getGreenwichMeanSiderealTime(time);
    #endif
}

decimal getGreenwichMeanSiderealTime(uint64_t epochs) {
    return 15 * (DECIMAL(18.697374558) + DECIMAL(24.06570982441908) *
            (getJulianDateTime(epochs) - J2000_JULIAN_DATE));
}

}  // namespace found
