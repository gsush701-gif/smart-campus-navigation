// CampusGraph.h
// Purpose: Undirected, unweighted campus graph stored as an adjacency list,
//          with BFS reachability, shortest path and connectivity reporting.
// Author:  Anish Dahal
#ifndef CAMPUS_GRAPH_H
#define CAMPUS_GRAPH_H

#include <cstddef>
#include <string>
#include <vector>

class CampusGraph {
public:
    // Creates an empty graph.
    CampusGraph();

    // Adds a building. Returns false if the name already exists ignoring case.
    bool addBuilding(const std::string& name);

    // Returns the index of a building ignoring case, or -1 if it is unknown.
    int findBuilding(const std::string& name) const;

    // Returns the stored spelling of the building at a valid index.
    const std::string& buildingName(int index) const;

    // Returns true if the two buildings are already directly connected.
    bool hasConnection(int a, int b) const;

    // Adds an undirected connection, updating both endpoints, counted once.
    void addConnection(int a, int b);

    // Displays each building with its neighbors in insertion order.
    void display() const;

    // Displays total buildings, total connections and every degree.
    void printCountsAndDegrees() const;

    // Displays BFS visit order with distances and the reachable count.
    void printReachable(int start) const;

    // Displays a path with the fewest connections, or a no-path message.
    void printShortestPath(int start, int destination) const;

    // Displays totals, highest degree, isolated buildings and components.
    void printConnectivityReport() const;

private:
    // Runs BFS from start and returns buildings in visit order. A building is
    // marked visited when it is added to the queue. The visited list is
    // supplied by the caller so that components can share it across runs.
    std::vector<int> bfs(int start, std::vector<bool>& visited,
                         std::vector<int>& distance,
                         std::vector<int>& parent) const;

    // Counts connected components with repeated BFS.
    std::size_t countComponents() const;

    std::vector<std::string> names_;            // insertion order
    std::vector<std::vector<int> > neighbors_;  // adjacency list
    std::size_t connectionCount_;               // each edge counted once
};

#endif
