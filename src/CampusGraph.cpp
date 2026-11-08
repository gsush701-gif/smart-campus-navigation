// CampusGraph.cpp
// Purpose: Implements the adjacency-list graph, BFS and graph reports.
// Author:  Anish Dahal
#include "CampusGraph.h"

#include <iostream>
#include <queue>

#include "StringUtils.h"

// Prints names separated by commas.
static void printNameList(const std::vector<std::string>& names) {
    for (std::size_t i = 0; i < names.size(); ++i) {
        std::cout << (i == 0 ? "" : ", ") << names[i];
    }
}

// Starts with no buildings and no connections.
CampusGraph::CampusGraph() : connectionCount_(0) {}

// Adds a vertex with an empty neighbor list unless the name is a duplicate.
bool CampusGraph::addBuilding(const std::string& name) {
    if (findBuilding(name) != -1) {
        return false;
    }
    names_.push_back(trim(name));
    neighbors_.push_back(std::vector<int>());
    return true;
}

// Scans the buildings for a case-insensitive match on the trimmed name.
int CampusGraph::findBuilding(const std::string& name) const {
    const std::string target = trim(name);
    for (std::size_t i = 0; i < names_.size(); ++i) {
        if (equalsIgnoreCase(names_[i], target)) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// Returns the spelling from buildings.txt.
const std::string& CampusGraph::buildingName(int index) const {
    return names_[index];
}

// Edges are stored at both endpoints, so checking one list also catches a
// reversed duplicate.
bool CampusGraph::hasConnection(int a, int b) const {
    for (std::size_t i = 0; i < neighbors_[a].size(); ++i) {
        if (neighbors_[a][i] == b) {
            return true;
        }
    }
    return false;
}

// Appends to both lists, preserving neighbor insertion order.
void CampusGraph::addConnection(int a, int b) {
    neighbors_[a].push_back(b);
    neighbors_[b].push_back(a);
    ++connectionCount_;
}

// Lists each building followed by its neighbors.
void CampusGraph::display() const {
    if (names_.empty()) {
        std::cout << "No buildings are loaded.\n";
        return;
    }
    for (std::size_t i = 0; i < names_.size(); ++i) {
        std::cout << names_[i] << ": ";
        if (neighbors_[i].empty()) {
            std::cout << "(no connections)";
        }
        for (std::size_t j = 0; j < neighbors_[i].size(); ++j) {
            std::cout << (j == 0 ? "" : ", ") << names_[neighbors_[i][j]];
        }
        std::cout << '\n';
    }
}

// Prints totals and the degree of every building.
void CampusGraph::printCountsAndDegrees() const {
    if (names_.empty()) {
        std::cout << "No buildings are loaded.\n";
    }
    std::cout << "Total buildings: " << names_.size() << '\n';
    std::cout << "Total connections: " << connectionCount_ << '\n';
    for (std::size_t i = 0; i < names_.size(); ++i) {
        std::cout << names_[i] << ": degree " << neighbors_[i].size() << '\n';
    }
}

// Standard BFS. Neighbors are taken in insertion order, so the first parent
// recorded for a building decides which equally short route is used.
std::vector<int> CampusGraph::bfs(int start, std::vector<bool>& visited,
                                  std::vector<int>& distance,
                                  std::vector<int>& parent) const {
    std::vector<int> order;
    std::queue<int> pending;
    visited[start] = true;
    distance[start] = 0;
    parent[start] = -1;
    pending.push(start);
    while (!pending.empty()) {
        const int current = pending.front();
        pending.pop();
        order.push_back(current);
        for (std::size_t i = 0; i < neighbors_[current].size(); ++i) {
            const int next = neighbors_[current][i];
            if (!visited[next]) {
                visited[next] = true;
                distance[next] = distance[current] + 1;
                parent[next] = current;
                pending.push(next);
            }
        }
    }
    return order;
}

// Prints the BFS visit order, distances and reachable count.
void CampusGraph::printReachable(int start) const {
    std::vector<bool> visited(names_.size(), false);
    std::vector<int> distance(names_.size(), -1);
    std::vector<int> parent(names_.size(), -1);
    const std::vector<int> order = bfs(start, visited, distance, parent);
    for (std::size_t i = 0; i < order.size(); ++i) {
        std::cout << names_[order[i]] << " (distance " << distance[order[i]] << ")\n";
    }
    std::cout << "Reachable: " << order.size() << " of " << names_.size()
              << " buildings\n";
}

// Rebuilds the route by following parents back from the destination.
void CampusGraph::printShortestPath(int start, int destination) const {
    std::vector<bool> visited(names_.size(), false);
    std::vector<int> distance(names_.size(), -1);
    std::vector<int> parent(names_.size(), -1);
    bfs(start, visited, distance, parent);
    if (!visited[destination]) {
        std::cout << "No path exists between " << names_[start] << " and "
                  << names_[destination] << ".\n";
        return;
    }
    std::vector<int> path;
    for (int current = destination; current != -1; current = parent[current]) {
        path.push_back(current);
    }
    for (std::size_t i = path.size(); i > 0; --i) {
        std::cout << names_[path[i - 1]] << '\n';
    }
    std::cout << "Number of connections: " << distance[destination] << '\n';
}

// Starts a new BFS from each building that no earlier BFS reached.
std::size_t CampusGraph::countComponents() const {
    std::vector<bool> visited(names_.size(), false);
    std::vector<int> distance(names_.size(), -1);
    std::vector<int> parent(names_.size(), -1);
    std::size_t components = 0;
    for (std::size_t i = 0; i < names_.size(); ++i) {
        if (!visited[i]) {
            bfs(static_cast<int>(i), visited, distance, parent);
            ++components;
        }
    }
    return components;
}

// Prints the connectivity summary from the shared adjacency list.
void CampusGraph::printConnectivityReport() const {
    if (names_.empty()) {
        std::cout << "No buildings are loaded.\n"
                  << "Total buildings: 0\nTotal connections: 0\n"
                  << "Connected components: 0\n";
        return;
    }
    std::size_t highest = 0;
    for (std::size_t i = 0; i < names_.size(); ++i) {
        if (neighbors_[i].size() > highest) {
            highest = neighbors_[i].size();
        }
    }
    std::vector<std::string> highestNames;
    std::vector<std::string> isolatedNames;
    for (std::size_t i = 0; i < names_.size(); ++i) {
        if (neighbors_[i].size() == highest) {
            highestNames.push_back(names_[i]);
        }
        if (neighbors_[i].empty()) {
            isolatedNames.push_back(names_[i]);
        }
    }
    std::cout << "Total buildings: " << names_.size() << '\n';
    std::cout << "Total connections: " << connectionCount_ << '\n';
    std::cout << "Highest degree (" << highest << "): ";
    printNameList(highestNames);
    std::cout << "\nIsolated buildings: ";
    if (isolatedNames.empty()) {
        std::cout << "None";
    }
    printNameList(isolatedNames);
    std::cout << "\nConnected components: " << countComponents() << '\n';
}
