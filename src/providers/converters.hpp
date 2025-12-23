#ifndef SRC_PROVIDERS_CONVERTERS_HPP_
#define SRC_PROVIDERS_CONVERTERS_HPP_

#include <stb_image/stb_image.h>

#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

#include "common/logging.hpp"
#include "common/time/time.hpp"
#include "common/spatial/attitude-utils.hpp"
#include "common/style.hpp"
#include "common/decimal.hpp"
#include "datafile/datafile.hpp"
#include "datafile/serialization.hpp"

// NOTE: Throwing exceptions is allowed in this file, as these functions
// must successfully parse data for any pipeline to function properly.
// If they fail, the pipeline should not be run, and an exception should be thrown.
// This is preferable than continuing with invalid data and outputting an unintended
// result.

namespace found {

/**
 * Converts a string to an unsigned char
 * 
 * @param str The string to convert
 * 
 * @return The unsigned char the string
 * represents
 * 
 * @pre str represents a number between 0
 * and 255
 */
inline unsigned char strtouc(const std::string &str) {
    return static_cast<unsigned char>(std::strtoul(str.c_str(), nullptr, 10));
}

inline size_t strtosize(const std::string &str) {
    return static_cast<size_t>(atoi(str.c_str()));
}

/**
 * Converts a string to a decimal
 * 
 * @param str The string to convert
 * 
 * @return The decimal numeber this string
 * represents
 * 
 * @pre The str must actually represent a decimal
 */
inline decimal strtodecimal(const std::string &str) {
    return STR_TO_DECIMAL(str);
}

/**
 * Converts a string to euler angles.
 * 
 * @param str The string to convert
 * 
 * @return An EulerAngle corresponding
 * to the string, if the str is valid
 * 
 * @pre The string must have 3 decimal
 * values seperated by commas or spaces
 */
inline EulerAngles strtoea(const std::string &str) {
    char delimiter = str.find(" ") != std::string::npos ? ' ' : ',';
    decimal result[3];

    size_t start = 0;
    size_t end = str.find(delimiter);
    size_t index = 0;

    while (index != 2 && end != std::string::npos) {
        result[index++] = strtodecimal(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }

    result[index++] = strtodecimal(str.substr(start));

    while (index != 3) result[index++] = 0;

    return EulerAngles(DegToRad(result[0]), DegToRad(result[1]), DegToRad(result[2]));
}

/**
 * Converts the string to a bool
 * 
 * @param str The string to convert
 * 
 * @return true iff the string represents
 * a true value
 */
inline bool strtobool(const std::string &str) {
    return str.size() != 0 && str != "0" && str != "false";
}

/**
 * Converts a string to an image
 * 
 * @param str The string to convert
 * 
 * @return The image that the string represents
 * 
 * @note This function uses stb_image.h to load the image
 * 
 * @throw std::runtime_error if the image cannot be loaded
 */
inline Image strtoimage(const std::string &str) {
    Image image;
    image.image = stbi_load(str.c_str(), &image.width, &image.height, &image.channels, 0);
    if (!image.image) {
        throw std::runtime_error("Could not load image " + str + ": " + stbi_failure_reason());
    }
    return image;
}

/**
 * Converts a string to time
 * 
 * @param str The string to convert
 * 
 * @return The time from epoch that the string represents (epochs in nanoseconds)
 */
inline DateTime strtodatetime(const std::string &str) {
    std::tm tm = {};
    std::istringstream ss(str);

    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        throw std::invalid_argument("Invalid datetime format: " + str);
    }

    // Store original values for validation
    int year = tm.tm_year + 1900;
    int month = tm.tm_mon + 1;
    int day = tm.tm_mday;
    int hour = tm.tm_hour;
    int minute = tm.tm_min;
    int second = tm.tm_sec;

    if (second > 59) {
        throw std::invalid_argument("Invalid second in datetime: " + str);
    }

    // Validate day of month (considering leap years)
    int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool is_leap_year = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (is_leap_year && month == 2) {
        days_in_month[1] = 29;
    }
    if (day > days_in_month[month - 1]) {
        throw std::invalid_argument("Invalid day in datetime: " + str);
    }

    std::time_t t = timegm(&tm);

    int nanosecond = 0;
    std::string nanos_str;
    if (std::getline(ss, nanos_str) && nanos_str.size() > 1 && nanos_str[0] == '.') {
        nanos_str = nanos_str.substr(1);
        nanos_str.resize(9, '0');  // pad or truncate to 9 digits
        nanosecond = std::stoi(nanos_str);
    }

    // Convert to nanoseconds: seconds * NS_PER_SEC + nanoseconds
    uint64_t epochs_ns = static_cast<uint64_t>(t) * NS_PER_SEC + static_cast<uint64_t>(nanosecond);

    return {
        epochs_ns,
        year,
        month,
        day,
        hour,
        minute,
        second,
        nanosecond
    };
}

/**
 * 
 */
inline DataFile strtodf(const std::string &str) {
    std::ifstream stream(str);
    return deserializeDataFile(stream, str);
}

/**
 * Converts a string to a vector of location records
 * 
 * @param str The string to convert
 * 
 * @return The vector of location records that the string represents
 * 
 * @pre str must refer to a file, either the Data File, or a space
 * delinated file with the following format on each line:
 * Timestamp(int) PositionX(decimal) PositionY(decimal) PositionZ(decimal)
 */
inline LocationRecords strtolr(const std::string &str) {
    if (str.size() >= 6) {
        if (str.substr(str.size() - 6) == ".found") {
            LOG_INFO("Getting Position Data from Data File (*.found)");
            DataFile data = strtodf(str);
            return LocationRecords(data.positions.get(), data.positions.get() + data.header.num_positions);
        }
    }

    LOG_INFO("Getting Position Data from non-Data File (not *.found)");
    LocationRecords records;
    std::ifstream file(str);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file " + str);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        LocationRecord record;
        if (!(iss >> record.timestamp >> record.position.x >> record.position.y >> record.position.z)) {
            file.close();
            throw std::runtime_error("Invalid format for file " + str + ": " + line);
        }
        records.push_back(record);
    }

    file.close();
    return records;
}

}  // namespace found

#endif  // SRC_PROVIDERS_CONVERTERS_HPP_
