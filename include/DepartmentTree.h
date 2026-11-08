// DepartmentTree.h
// Purpose: General tree of academic departments, in which a node may have any
//          number of children, with search, traversals and statistics.
// Author:  Vijay Ram Sanvi Bhaskarla
#ifndef DEPARTMENT_TREE_H
#define DEPARTMENT_TREE_H

#include <cstddef>
#include <string>
#include <vector>

// One department. Children are kept in insertion order.
struct DepartmentNode {
    std::string name;
    DepartmentNode* parent;
    std::vector<DepartmentNode*> children;

    // Creates a node with no children.
    DepartmentNode(const std::string& nodeName, DepartmentNode* parentNode);
};

class DepartmentTree {
public:
    // Outcome of adding a department below an existing parent.
    enum AddResult { ADDED, UNKNOWN_PARENT, DUPLICATE_NAME };

    // Creates an empty tree.
    DepartmentTree();

    // Releases every node.
    ~DepartmentTree();

    // Returns true if the tree has no root.
    bool empty() const;

    // Creates the root of an empty tree. Returns false if a root exists.
    bool addRoot(const std::string& name);

    // Adds a child under an existing parent, leaving the tree unchanged on
    // failure.
    AddResult addChild(const std::string& name, const std::string& parentName);

    // Returns the stored spelling of a department matched ignoring case, or
    // an empty string if there is no such department.
    std::string storedName(const std::string& name) const;

    // Displays name, parent, direct-child count and depth, or not found.
    void printSearch(const std::string& name) const;

    // Displays the preorder hierarchy, indented four spaces per depth.
    void printHierarchy() const;

    // Returns department names in preorder: each parent before its children.
    std::vector<std::string> preorder() const;

    // Returns department names in postorder: each parent after its children.
    std::vector<std::string> postorder() const;

    // Returns department names one depth at a time, using a queue.
    std::vector<std::string> levelOrder() const;

    // Displays total departments, leaves and height.
    void printStatistics() const;

private:
    // Copying is disabled because the tree owns raw pointers; a copy would
    // free the same nodes twice. These are declared but never defined.
    DepartmentTree(const DepartmentTree&);

    // Assignment is disabled for the same reason as copying.
    DepartmentTree& operator=(const DepartmentTree&);

    // Returns the node with this name ignoring case, or a null pointer.
    DepartmentNode* find(const std::string& name) const;

    // Searches the subtree rooted at node, in preorder.
    static DepartmentNode* findIn(DepartmentNode* node, const std::string& name);

    // Frees the subtree rooted at node, children before their parent.
    static void destroy(DepartmentNode* node);

    // Prints the subtree rooted at node, indented four spaces per depth.
    static void printIndented(const DepartmentNode* node, std::size_t depth);

    // Appends the subtree's names in preorder.
    static void collectPreorder(const DepartmentNode* node, std::vector<std::string>& names);

    // Appends the subtree's names in postorder.
    static void collectPostorder(const DepartmentNode* node, std::vector<std::string>& names);

    // Adds the subtree to the running total, leaf count and height.
    static void gatherStatistics(const DepartmentNode* node, std::size_t depth,
                                 std::size_t& total, std::size_t& leaves,
                                 std::size_t& height);

    DepartmentNode* root_;
};

#endif
