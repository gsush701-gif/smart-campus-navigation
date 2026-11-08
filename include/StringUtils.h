// StringUtils.h
// Purpose: Text helpers shared by every module (trimming, case handling,
//          field splitting, facility ID validation).
// Author:  Sushil Gautam, Anish Dahal, Vijay Ram Sanvi Bhaskarla
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <vector>

// Removes spaces, tabs, carriage returns and other whitespace from both ends.
std::string trim(const std::string& text);

// Returns a copy with ASCII letters converted to uppercase.
std::string toUpperAscii(const std::string& text);

// Returns a copy with ASCII letters converted to lowercase.
std::string toLowerAscii(const std::string& text);

// Compares two strings ignoring ASCII capitalization. Internal spaces matter.
bool equalsIgnoreCase(const std::string& a, const std::string& b);

// Splits a line on the delimiter and trims each field. Empty fields are kept,
// so "A|" produces two fields.
std::vector<std::string> splitFields(const std::string& line, char delimiter);

// Trims and uppercases a facility ID. Validate only after calling this.
std::string normalizeFacilityId(const std::string& id);

// Returns true for a normalized ID: F followed by exactly three digits.
bool isValidFacilityId(const std::string& normalizedId);

#endif
