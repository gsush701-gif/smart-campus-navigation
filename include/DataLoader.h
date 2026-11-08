// DataLoader.h
// Purpose: Reads and validates the four input files, printing a warning for
//          each rejected record and a load summary after each file.
// Author:  Sushil Gautam, Anish Dahal, Vijay Ram Sanvi Bhaskarla
#ifndef DATA_LOADER_H
#define DATA_LOADER_H

#include <string>

#include "CampusGraph.h"
#include "DepartmentTree.h"
#include "Facility.h"

// Loads buildings.txt from the data directory into the graph.
void loadBuildings(const std::string& directory, CampusGraph& graph);

// Loads connections.txt; buildings must already be loaded.
void loadConnections(const std::string& directory, CampusGraph& graph);

// Loads facilities.txt; buildings must already be loaded.
void loadFacilities(const std::string& directory, const CampusGraph& graph,
                    FacilityDatabase& facilities);

// Loads departments.txt into the tree.
void loadDepartments(const std::string& directory, DepartmentTree& tree);

#endif
