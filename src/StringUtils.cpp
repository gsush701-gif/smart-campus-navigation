// StringUtils.cpp
// Purpose: Implements the shared text helpers declared in StringUtils.h.
// Author:  Sushil Gautam, Anish Dahal, Vijay Ram Sanvi Bhaskarla
#include "StringUtils.h"

// Characters treated as whitespace around fields and user input.
static const char* const WHITESPACE = " \t\r\n\f\v";

// Removes whitespace from both ends of the text.
std::string trim(const std::string& text) {
    const std::string::size_type first = text.find_first_not_of(WHITESPACE);
    if (first == std::string::npos) {
        return "";
    }
    const std::string::size_type last = text.find_last_not_of(WHITESPACE);
    return text.substr(first, last - first + 1);
}

// Uppercases ASCII letters only, so results do not depend on the locale.
std::string toUpperAscii(const std::string& text) {
    std::string result = text;
    for (std::string::size_type i = 0; i < result.size(); ++i) {
        if (result[i] >= 'a' && result[i] <= 'z') {
            result[i] = static_cast<char>(result[i] - 'a' + 'A');
        }
    }
    return result;
}

// Lowercases ASCII letters only, so results do not depend on the locale.
std::string toLowerAscii(const std::string& text) {
    std::string result = text;
    for (std::string::size_type i = 0; i < result.size(); ++i) {
        if (result[i] >= 'A' && result[i] <= 'Z') {
            result[i] = static_cast<char>(result[i] - 'A' + 'a');
        }
    }
    return result;
}

// Case-insensitive equality; lengths must match, so internal spaces count.
bool equalsIgnoreCase(const std::string& a, const std::string& b) {
    return a.size() == b.size() && toLowerAscii(a) == toLowerAscii(b);
}

// Splits on the delimiter, trimming each field and keeping empty fields.
std::vector<std::string> splitFields(const std::string& line, char delimiter) {
    std::vector<std::string> fields;
    std::string::size_type start = 0;
    while (true) {
        const std::string::size_type end = line.find(delimiter, start);
        if (end == std::string::npos) {
            fields.push_back(trim(line.substr(start)));
            return fields;
        }
        fields.push_back(trim(line.substr(start, end - start)));
        start = end + 1;
    }
}

// Applies the required normalization: trim first, then uppercase.
std::string normalizeFacilityId(const std::string& id) {
    return toUpperAscii(trim(id));
}

// Checks the F### format on an already normalized ID.
bool isValidFacilityId(const std::string& normalizedId) {
    if (normalizedId.size() != 4 || normalizedId[0] != 'F') {
        return false;
    }
    for (std::string::size_type i = 1; i < normalizedId.size(); ++i) {
        if (normalizedId[i] < '0' || normalizedId[i] > '9') {
            return false;
        }
    }
    return true;
}
