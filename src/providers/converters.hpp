#ifndef CONVERTERS_H
#define CONVERTERS_H

#include <iostream>
#include <string>

#include "common/spatial/attitude-utils.hpp"
#include "common/decimal.hpp"

namespace found {

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

    while (end != std::string::npos) {
        result[index++] = strtodecimal(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }

    result[index] = strtodecimal(str.substr(start));

    return EulerAngles(result[0], result[1], result[2]);
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

}  // namespace found

#endif  // CONVERTERS_H
