// DataLoader.cpp
// Purpose: Implements file reading and the Section 5 validation rules. Each
//          loader checks a record in this order: field count, empty fields,
//          ID format, unknown reference, then duplicate.
// Author:  Sushil Gautam, Anish Dahal, Vijay Ram Sanvi Bhaskarla
#include "DataLoader.h"

#include <fstream>
#include <iostream>
#include <vector>

#include "StringUtils.h"

namespace {

// Accepted and rejected record counts for one file.
struct LoadCounts {
    int accepted;
    int rejected;
};

// Joins the data directory and a file name.
std::string joinPath(const std::string& directory, const std::string& fileName) {
    if (directory.empty() || directory[directory.size() - 1] == '/') {
        return directory + fileName;
    }
    return directory + "/" + fileName;
}

// Returns true if any field is empty.
bool hasEmptyField(const std::vector<std::string>& fields) {
    for (std::size_t i = 0; i < fields.size(); ++i) {
        if (fields[i].empty()) {
            return true;
        }
    }
    return false;
}

// Builds the standard wrong-field-count reason.
std::string fieldCountReason(std::size_t expected, std::size_t actual) {
    return "expected " + std::to_string(expected) + " field(s), found " +
           std::to_string(actual);
}

// Interface implemented once per input file.
class RecordHandler {
public:
    // Allows handlers to be used through the base class.
    virtual ~RecordHandler() {}

    // Validates and stores one record. Returns an empty string if it was
    // accepted, or the rejection reason.
    virtual std::string handle(const std::vector<std::string>& fields) = 0;
};

// Reads every record of a file, skipping blank and comment lines, and passes
// it to the handler. A file that cannot be opened is treated as empty.
LoadCounts readRecords(const std::string& directory, const std::string& fileName,
                       RecordHandler& handler) {
    LoadCounts counts = {0, 0};
    const std::string path = joinPath(directory, fileName);
    std::ifstream input(path.c_str());
    if (!input.is_open()) {
        std::cout << "ERROR: cannot open file " << path << "; treating it as empty.\n";
        return counts;
    }
    std::string line;
    int lineNumber = 0;
    while (std::getline(input, line)) {
        ++lineNumber;
        // Some Windows editors begin a file with a UTF-8 byte order mark.
        if (lineNumber == 1 && line.compare(0, 3, "\xEF\xBB\xBF") == 0) {
            line.erase(0, 3);
        }
        const std::string text = trim(line);
        if (text.empty() || text[0] == '#') {
            continue;
        }
        const std::string reason = handler.handle(splitFields(text, '|'));
        if (reason.empty()) {
            ++counts.accepted;
        } else {
            ++counts.rejected;
            std::cout << "WARNING: " << fileName << " line " << lineNumber << ": "
                      << reason << '\n';
        }
    }
    return counts;
}

// Prints the first part of a load summary, without ending the line.
void printSummary(const std::string& fileName, const LoadCounts& counts) {
    std::cout << fileName << ": " << counts.accepted << " accepted, "
              << counts.rejected << " rejected";
}

// Validates buildings.txt records.
class BuildingHandler : public RecordHandler {
public:
    // Stores the graph that receives accepted buildings.
    explicit BuildingHandler(CampusGraph& graph) : graph_(graph) {}

    // Checks the field count, then adds the building unless it is a duplicate.
    std::string handle(const std::vector<std::string>& fields) {
        if (fields.size() != 1) {
            return fieldCountReason(1, fields.size());
        }
        if (!graph_.addBuilding(fields[0])) {
            return "duplicate building \"" + fields[0] + "\"";
        }
        return "";
    }

private:
    CampusGraph& graph_;
};

// Validates connections.txt records.
class ConnectionHandler : public RecordHandler {
public:
    // Stores the graph that receives accepted connections.
    explicit ConnectionHandler(CampusGraph& graph) : graph_(graph) {}

    // Rejects bad fields, unknown buildings, self-connections and duplicates,
    // including reversed duplicates, then adds the connection.
    std::string handle(const std::vector<std::string>& fields) {
        if (fields.size() != 2) {
            return fieldCountReason(2, fields.size());
        }
        if (hasEmptyField(fields)) {
            return "empty building name";
        }
        const int a = graph_.findBuilding(fields[0]);
        const int b = graph_.findBuilding(fields[1]);
        if (a == -1) {
            return "unknown building \"" + fields[0] + "\"";
        }
        if (b == -1) {
            return "unknown building \"" + fields[1] + "\"";
        }
        if (a == b) {
            return "self-connection on \"" + graph_.buildingName(a) + "\"";
        }
        if (graph_.hasConnection(a, b)) {
            return "duplicate connection between \"" + graph_.buildingName(a) +
                   "\" and \"" + graph_.buildingName(b) + "\"";
        }
        graph_.addConnection(a, b);
        return "";
    }

private:
    CampusGraph& graph_;
};

// Validates facilities.txt records.
class FacilityHandler : public RecordHandler {
public:
    // Stores the graph used for building checks and the target database.
    FacilityHandler(const CampusGraph& graph, FacilityDatabase& facilities)
        : graph_(graph), facilities_(facilities) {}

    // Rejects bad fields, invalid IDs, unknown buildings and duplicate IDs,
    // then adds the facility using the spelling from buildings.txt.
    std::string handle(const std::vector<std::string>& fields) {
        if (fields.size() != 4) {
            return fieldCountReason(4, fields.size());
        }
        if (hasEmptyField(fields)) {
            return "empty required field";
        }
        Facility facility;
        facility.id = normalizeFacilityId(fields[0]);
        if (!isValidFacilityId(facility.id)) {
            return "invalid ID \"" + fields[0] + "\": must be F plus exactly three digits";
        }
        const int building = graph_.findBuilding(fields[2]);
        if (building == -1) {
            return "unknown building \"" + fields[2] + "\"";
        }
        facility.name = fields[1];
        facility.building = graph_.buildingName(building);
        facility.category = fields[3];
        if (!facilities_.add(facility)) {
            return "duplicate facility ID " + facility.id;
        }
        return "";
    }

private:
    const CampusGraph& graph_;
    FacilityDatabase& facilities_;
};

// Validates departments.txt records.
class DepartmentHandler : public RecordHandler {
public:
    // Stores the tree that receives accepted departments.
    explicit DepartmentHandler(DepartmentTree& tree) : tree_(tree) {}

    // Creates the root from the first valid record, then requires each later
    // department to have an existing parent and a unique name.
    std::string handle(const std::vector<std::string>& fields) {
        if (fields.size() != 2) {
            return fieldCountReason(2, fields.size());
        }
        if (hasEmptyField(fields)) {
            return "empty required field";
        }
        if (tree_.empty()) {
            if (!equalsIgnoreCase(fields[1], "NONE")) {
                return "the first department must be the root, with parent NONE";
            }
            tree_.addRoot(fields[0]);
            return "";
        }
        switch (tree_.addChild(fields[0], fields[1])) {
            case DepartmentTree::UNKNOWN_PARENT:
                return "unknown parent \"" + fields[1] + "\"";
            case DepartmentTree::DUPLICATE_NAME:
                return "duplicate department \"" + fields[0] + "\"";
            default:
                return "";
        }
    }

private:
    DepartmentTree& tree_;
};

}  // namespace

// Loads buildings and prints their summary.
void loadBuildings(const std::string& directory, CampusGraph& graph) {
    BuildingHandler handler(graph);
    printSummary("buildings.txt", readRecords(directory, "buildings.txt", handler));
    std::cout << '\n';
}

// Loads connections and prints their summary.
void loadConnections(const std::string& directory, CampusGraph& graph) {
    ConnectionHandler handler(graph);
    printSummary("connections.txt", readRecords(directory, "connections.txt", handler));
    std::cout << '\n';
}

// Loads facilities and prints their summary with the hash table diagnostic.
void loadFacilities(const std::string& directory, const CampusGraph& graph,
                    FacilityDatabase& facilities) {
    FacilityHandler handler(graph, facilities);
    printSummary("facilities.txt", readRecords(directory, "facilities.txt", handler));
    const HashTable& table = facilities.table();
    std::cout << " (hash capacity " << table.capacity() << ", entries "
              << table.entryCount() << ", collisions " << table.collisionCount()
              << ")\n";
}

// Loads departments and prints their summary.
void loadDepartments(const std::string& directory, DepartmentTree& tree) {
    DepartmentHandler handler(tree);
    printSummary("departments.txt", readRecords(directory, "departments.txt", handler));
    std::cout << '\n';
}
