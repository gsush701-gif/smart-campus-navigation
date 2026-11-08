// main.cpp
// Purpose: Program entry point. Coordinates file loading and runs the menu.
//          Builds the Milestone 1 menu by default, or the Final menu when
//          compiled with -DFINAL_MENU.
// Author:  Sushil Gautam, Anish Dahal, Vijay Ram Sanvi Bhaskarla
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "CampusGraph.h"
#include "DataLoader.h"
#include "DepartmentTree.h"
#include "Facility.h"
#include "StringUtils.h"

namespace {

// The three data structures that the menu operates on.
struct Campus {
    CampusGraph graph;
    FacilityDatabase facilities;
    DepartmentTree departments;
};

// One numbered menu option.
struct MenuItem {
    std::string label;
    void (*action)(Campus&);
};

// Prompts and reads one trimmed line. End of input yields an empty string.
std::string promptLine(const std::string& prompt) {
    std::cout << prompt << std::flush;
    std::string line;
    if (!std::getline(std::cin, line)) {
        return "";
    }
    return trim(line);
}

// Prints the blank-input error shared by every name prompt.
void printBlankError(const std::string& what) {
    std::cout << "Error: " << what << " cannot be blank.\n";
}

// Prints one name per line.
void printSequence(const std::vector<std::string>& names) {
    if (names.empty()) {
        std::cout << "No departments are loaded.\n";
    }
    for (std::size_t i = 0; i < names.size(); ++i) {
        std::cout << names[i] << '\n';
    }
}

// Menu action: list all facilities.
void viewFacilities(Campus& campus) {
    campus.facilities.displayAll();
}

// Menu action: search for a facility by ID.
void searchFacilityById(Campus& campus) {
    const std::string id = promptLine("Enter facility ID: ");
    if (id.empty()) {
        printBlankError("Facility ID");
        return;
    }
    campus.facilities.searchById(id);
}

// Menu action: search for facilities by complete name.
void searchFacilityByName(Campus& campus) {
    const std::string name = promptLine("Enter facility name: ");
    if (name.empty()) {
        printBlankError("Facility name");
        return;
    }
    campus.facilities.searchByName(name);
}

// Menu action: search for facilities by complete category.
void searchFacilityByCategory(Campus& campus) {
    const std::string category = promptLine("Enter category: ");
    if (category.empty()) {
        printBlankError("Category");
        return;
    }
    campus.facilities.searchByCategory(category);
}

// Menu action: display the graph.
void viewGraph(Campus& campus) {
    campus.graph.display();
}

// Menu action: display graph totals and degrees.
void viewGraphCounts(Campus& campus) {
    campus.graph.printCountsAndDegrees();
}

// Menu action: run BFS from a chosen building.
void findReachable(Campus& campus) {
    const std::string name = promptLine("Enter starting building: ");
    if (name.empty()) {
        printBlankError("Building name");
        return;
    }
    const int start = campus.graph.findBuilding(name);
    if (start == -1) {
        std::cout << "Error: unknown building \"" << name << "\".\n";
        return;
    }
    campus.graph.printReachable(start);
}

// Menu action: find a shortest path. Both names are read before checking, so
// that every unknown building can be identified.
void findShortestPath(Campus& campus) {
    const std::string startName = promptLine("Enter starting building: ");
    if (startName.empty()) {
        printBlankError("Building name");
        return;
    }
    const std::string endName = promptLine("Enter destination building: ");
    if (endName.empty()) {
        printBlankError("Building name");
        return;
    }
    const int start = campus.graph.findBuilding(startName);
    const int destination = campus.graph.findBuilding(endName);
    if (start == -1) {
        std::cout << "Error: unknown building \"" << startName << "\".\n";
    }
    if (destination == -1) {
        std::cout << "Error: unknown building \"" << endName << "\".\n";
    }
    if (start != -1 && destination != -1) {
        campus.graph.printShortestPath(start, destination);
    }
}

// Menu action: add a department in memory. An empty tree takes the new
// department as its root without asking for a parent.
void addDepartment(Campus& campus) {
    const std::string name = promptLine("Enter department name: ");
    if (name.empty()) {
        printBlankError("Department name");
        return;
    }
    if (campus.departments.empty()) {
        campus.departments.addRoot(name);
        std::cout << "Added \"" << name << "\" as the root department.\n";
        return;
    }
    const std::string parent = promptLine("Enter parent department: ");
    if (parent.empty()) {
        printBlankError("Parent department");
        return;
    }
    switch (campus.departments.addChild(name, parent)) {
        case DepartmentTree::ADDED:
            std::cout << "Added \"" << name << "\" under \""
                      << campus.departments.storedName(parent) << "\".\n";
            break;
        case DepartmentTree::UNKNOWN_PARENT:
            std::cout << "Error: unknown parent \"" << parent << "\".\n";
            break;
        case DepartmentTree::DUPLICATE_NAME:
            std::cout << "Error: department \"" << name << "\" already exists.\n";
            break;
    }
}

// Menu action: search for a department.
void searchDepartment(Campus& campus) {
    const std::string name = promptLine("Enter department name: ");
    if (name.empty()) {
        printBlankError("Department name");
        return;
    }
    campus.departments.printSearch(name);
}

// Menu action: display the indented hierarchy.
void viewHierarchy(Campus& campus) {
    campus.departments.printHierarchy();
}

// Menu action: display the preorder sequence.
void viewPreorder(Campus& campus) {
    printSequence(campus.departments.preorder());
}

#ifdef FINAL_MENU
// Menu action: display the postorder sequence.
void viewPostorder(Campus& campus) {
    printSequence(campus.departments.postorder());
}

// Menu action: display the level-order sequence.
void viewLevelOrder(Campus& campus) {
    printSequence(campus.departments.levelOrder());
}

// Menu action: display department statistics.
void viewDepartmentStatistics(Campus& campus) {
    campus.departments.printStatistics();
}

// Menu action: display the category report.
void viewCategoryReport(Campus& campus) {
    campus.facilities.printCategoryReport();
}

// Menu action: display the connectivity report.
void viewConnectivityReport(Campus& campus) {
    campus.graph.printConnectivityReport();
}
#endif

// Adds an option to the menu.
void addItem(std::vector<MenuItem>& menu, const std::string& label,
             void (*action)(Campus&)) {
    MenuItem item;
    item.label = label;
    item.action = action;
    menu.push_back(item);
}

// Builds the options in the numbering required by Section 6.9.
std::vector<MenuItem> buildMenu() {
    std::vector<MenuItem> menu;
#ifndef FINAL_MENU
    addItem(menu, "View all facilities", viewFacilities);
    addItem(menu, "Search facility by ID", searchFacilityById);
    addItem(menu, "View campus graph", viewGraph);
    addItem(menu, "Display graph counts and degrees", viewGraphCounts);
    addItem(menu, "Find reachable buildings (BFS)", findReachable);
    addItem(menu, "Add department", addDepartment);
    addItem(menu, "Search department", searchDepartment);
    addItem(menu, "Display department hierarchy", viewHierarchy);
    addItem(menu, "Run preorder traversal", viewPreorder);
    // Referenced here so the Milestone 1 build has no unused functions.
    (void)searchFacilityByName;
    (void)searchFacilityByCategory;
    (void)findShortestPath;
#else
    addItem(menu, "View all facilities", viewFacilities);
    addItem(menu, "Search facility by ID", searchFacilityById);
    addItem(menu, "Search facility by name", searchFacilityByName);
    addItem(menu, "Search facility by category", searchFacilityByCategory);
    addItem(menu, "View campus graph", viewGraph);
    addItem(menu, "Display graph counts and degrees", viewGraphCounts);
    addItem(menu, "Find reachable buildings (BFS)", findReachable);
    addItem(menu, "Find shortest path (BFS)", findShortestPath);
    addItem(menu, "Add department", addDepartment);
    addItem(menu, "Search department", searchDepartment);
    addItem(menu, "Display department hierarchy", viewHierarchy);
    addItem(menu, "Preorder traversal", viewPreorder);
    addItem(menu, "Postorder traversal", viewPostorder);
    addItem(menu, "Level-order traversal", viewLevelOrder);
    addItem(menu, "Department statistics", viewDepartmentStatistics);
    addItem(menu, "Facility category report", viewCategoryReport);
    addItem(menu, "Graph connectivity report", viewConnectivityReport);
#endif
    return menu;
}

// Converts a menu entry to a number. Returns -1 unless the whole entry is
// digits and within range. Reading full lines means bad input can never leave
// the stream in a failed state or leave text behind for the next prompt.
int parseChoice(const std::string& text, std::size_t optionCount) {
    if (text.empty() || text.size() > 4) {
        return -1;
    }
    int value = 0;
    for (std::string::size_type i = 0; i < text.size(); ++i) {
        if (text[i] < '0' || text[i] > '9') {
            return -1;
        }
        value = value * 10 + (text[i] - '0');
    }
    return value <= static_cast<int>(optionCount) ? value : -1;
}

// Shows the menu and runs options until Exit or the end of input.
void runMenu(Campus& campus) {
    const std::vector<MenuItem> menu = buildMenu();
    while (true) {
        std::cout << "\n===== Smart Campus Menu =====\n";
        for (std::size_t i = 0; i < menu.size(); ++i) {
            std::cout << i + 1 << ". " << menu[i].label << '\n';
        }
        std::cout << "0. Exit\nEnter choice: " << std::flush;

        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cout << "\nEnd of input. Goodbye.\n";
            return;
        }
        const int choice = parseChoice(trim(line), menu.size());
        if (choice == -1) {
            std::cout << "Error: enter a number from 0 to " << menu.size() << ".\n";
        } else if (choice == 0) {
            std::cout << "Goodbye.\n";
            return;
        } else {
            std::cout << '\n';
            menu[choice - 1].action(campus);
        }
    }
}

}  // namespace

// Loads the data directory given as the optional argument, then runs the menu.
int main(int argc, char* argv[]) {
    const std::string directory = argc > 1 ? argv[1] : "data/";
    Campus campus;
    loadBuildings(directory, campus.graph);
    loadConnections(directory, campus.graph);
    loadFacilities(directory, campus.graph, campus.facilities);
    loadDepartments(directory, campus.departments);
    runMenu(campus);
    return 0;
}
