// Facility.cpp
// Purpose: Implements facility storage, searches and the category report.
// Author:  Sushil Gautam
#include "Facility.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "StringUtils.h"

// Prints one facility on a single line.
static void printFacility(const Facility& facility) {
    std::cout << facility.id << " | " << facility.name << " | "
              << facility.building << " | " << facility.category << '\n';
}

// Stores the facility and indexes it by ID. The hash table is updated first
// because it is what detects duplicate IDs.
bool FacilityDatabase::add(const Facility& facility) {
    if (!table_.insert(facility.id, facilities_.size())) {
        return false;
    }
    Facility stored = facility;
    stored.id = normalizeFacilityId(facility.id);
    stored.category = recordCategory(facility.category);
    facilities_.push_back(stored);
    return true;
}

// Finds or creates the category entry and increases its count.
std::string FacilityDatabase::recordCategory(const std::string& category) {
    for (std::size_t i = 0; i < categories_.size(); ++i) {
        if (equalsIgnoreCase(categories_[i].name, category)) {
            ++categories_[i].count;
            return categories_[i].name;
        }
    }
    CategoryCount entry;
    entry.name = category;
    entry.count = 1;
    categories_.push_back(entry);
    return category;
}

// Exposes the table for diagnostics.
const HashTable& FacilityDatabase::table() const {
    return table_;
}

// Lists all facilities in file order, or an empty-state message.
void FacilityDatabase::displayAll() const {
    if (facilities_.empty()) {
        std::cout << "No facilities are loaded.\n";
    }
    for (std::size_t i = 0; i < facilities_.size(); ++i) {
        printFacility(facilities_[i]);
    }
    std::cout << "Total facilities: " << facilities_.size() << '\n';
}

// Reports one of three outcomes: invalid format, not found, or found.
void FacilityDatabase::searchById(const std::string& input) const {
    const std::string id = normalizeFacilityId(input);
    if (!isValidFacilityId(id)) {
        std::cout << "Invalid format: an ID is F followed by exactly three digits.\n";
        return;
    }
    std::size_t index = 0;
    if (!table_.find(id, index)) {
        std::cout << "Not found: no facility with ID " << id << ".\n";
        return;
    }
    std::cout << "Found: ";
    printFacility(facilities_[index]);
}

// Scans the records for complete name matches.
void FacilityDatabase::searchByName(const std::string& name) const {
    const std::string target = trim(name);
    std::size_t matches = 0;
    for (std::size_t i = 0; i < facilities_.size(); ++i) {
        if (equalsIgnoreCase(facilities_[i].name, target)) {
            printFacility(facilities_[i]);
            ++matches;
        }
    }
    if (matches == 0) {
        std::cout << "No facilities found with name \"" << target << "\".\n";
    }
    std::cout << "Matches: " << matches << '\n';
}

// Scans the records for complete category matches.
void FacilityDatabase::searchByCategory(const std::string& category) const {
    const std::string target = trim(category);
    std::size_t matches = 0;
    for (std::size_t i = 0; i < facilities_.size(); ++i) {
        if (equalsIgnoreCase(facilities_[i].category, target)) {
            printFacility(facilities_[i]);
            ++matches;
        }
    }
    if (matches == 0) {
        std::cout << "No facilities found in category \"" << target << "\".\n";
    }
    std::cout << "Matches: " << matches << '\n';
}

// Prints each category with its share of all facilities. The empty case
// returns before any division takes place.
void FacilityDatabase::printCategoryReport() const {
    if (facilities_.empty()) {
        std::cout << "No facilities are available.\nTotal: 0\n";
        return;
    }
    const double total = static_cast<double>(facilities_.size());
    for (std::size_t i = 0; i < categories_.size(); ++i) {
        std::ostringstream percentage;
        percentage << std::fixed << std::setprecision(1)
                   << 100.0 * static_cast<double>(categories_[i].count) / total;
        std::cout << categories_[i].name << " | " << categories_[i].count << " | "
                  << percentage.str() << "%\n";
    }
    std::cout << "Total | " << facilities_.size() << " | 100.0%\n";
}
