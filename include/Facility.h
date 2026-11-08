// Facility.h
// Purpose: The facility record and the database that stores facilities in
//          file order, indexes them by ID, and answers searches and reports.
// Author:  Sushil Gautam
#ifndef FACILITY_H
#define FACILITY_H

#include <cstddef>
#include <string>
#include <vector>

#include "HashTable.h"

// One campus facility. The ID is uppercase and the building uses the
// spelling from buildings.txt.
struct Facility {
    std::string id;
    std::string name;
    std::string building;
    std::string category;
};

class FacilityDatabase {
public:
    // Adds a validated facility. Returns false if the ID is a duplicate, in
    // which case nothing changes, including the recorded category spellings.
    bool add(const Facility& facility);

    // Read-only access to the hash table for the HASH-7 diagnostic.
    const HashTable& table() const;

    // Displays every facility in file order with the total.
    void displayAll() const;

    // Normalizes the entered ID and looks it up in the hash table.
    void searchById(const std::string& input) const;

    // Displays exact name matches, ignoring case, in file order.
    void searchByName(const std::string& name) const;

    // Displays exact category matches, ignoring case, in file order.
    void searchByCategory(const std::string& category) const;

    // Displays the count and percentage of facilities in each category.
    void printCategoryReport() const;

private:
    // A distinct category: its first accepted spelling and facility count.
    struct CategoryCount {
        std::string name;
        std::size_t count;
    };

    // Records one more facility in a category and returns the first
    // accepted spelling of that category.
    std::string recordCategory(const std::string& category);

    std::vector<Facility> facilities_;       // file order
    std::vector<CategoryCount> categories_;  // first-seen order
    HashTable table_;                        // ID -> index in facilities_
};

#endif
