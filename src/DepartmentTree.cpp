// DepartmentTree.cpp
// Purpose: Implements the department general tree and its traversals.
// Author:  Vijay Ram Sanvi Bhaskarla
#include "DepartmentTree.h"

#include <iostream>
#include <queue>

#include "StringUtils.h"

// Stores the name and parent link.
DepartmentNode::DepartmentNode(const std::string& nodeName, DepartmentNode* parentNode)
    : name(nodeName), parent(parentNode) {}

// Starts with no root.
DepartmentTree::DepartmentTree() : root_(NULL) {}

// Frees all nodes.
DepartmentTree::~DepartmentTree() {
    destroy(root_);
}

// Deletes children before their parent.
void DepartmentTree::destroy(DepartmentNode* node) {
    if (node == NULL) {
        return;
    }
    for (std::size_t i = 0; i < node->children.size(); ++i) {
        destroy(node->children[i]);
    }
    delete node;
}

// An empty tree has no root.
bool DepartmentTree::empty() const {
    return root_ == NULL;
}

// Creates the root if there is none.
bool DepartmentTree::addRoot(const std::string& name) {
    if (root_ != NULL) {
        return false;
    }
    root_ = new DepartmentNode(trim(name), NULL);
    return true;
}

// Validates the parent and name, then appends the new child.
DepartmentTree::AddResult DepartmentTree::addChild(const std::string& name,
                                                   const std::string& parentName) {
    DepartmentNode* parent = find(parentName);
    if (parent == NULL) {
        return UNKNOWN_PARENT;
    }
    if (find(name) != NULL) {
        return DUPLICATE_NAME;
    }
    parent->children.push_back(new DepartmentNode(trim(name), parent));
    return ADDED;
}

// Searches the whole tree for a trimmed, case-insensitive match.
DepartmentNode* DepartmentTree::find(const std::string& name) const {
    return findIn(root_, trim(name));
}

// Looks up a department and returns the spelling it was added with.
std::string DepartmentTree::storedName(const std::string& name) const {
    const DepartmentNode* node = find(name);
    return node == NULL ? "" : node->name;
}

// Preorder search of a subtree.
DepartmentNode* DepartmentTree::findIn(DepartmentNode* node, const std::string& name) {
    if (node == NULL || equalsIgnoreCase(node->name, name)) {
        return node;
    }
    for (std::size_t i = 0; i < node->children.size(); ++i) {
        DepartmentNode* match = findIn(node->children[i], name);
        if (match != NULL) {
            return match;
        }
    }
    return NULL;
}

// Prints the details of one department; depth is found by walking to the root.
void DepartmentTree::printSearch(const std::string& name) const {
    if (root_ == NULL) {
        std::cout << "No departments are loaded.\n";
        return;
    }
    const DepartmentNode* node = find(name);
    if (node == NULL) {
        std::cout << "Department \"" << trim(name) << "\" was not found.\n";
        return;
    }
    std::size_t depth = 0;
    for (const DepartmentNode* up = node->parent; up != NULL; up = up->parent) {
        ++depth;
    }
    std::cout << "Name: " << node->name << '\n';
    std::cout << "Parent: " << (node->parent == NULL ? "None" : node->parent->name) << '\n';
    std::cout << "Direct children: " << node->children.size() << '\n';
    std::cout << "Depth: " << depth << '\n';
}

// Prints the tree, or an empty-state message.
void DepartmentTree::printHierarchy() const {
    if (root_ == NULL) {
        std::cout << "No departments are loaded.\n";
        return;
    }
    printIndented(root_, 0);
}

// Prints a node at its depth, then its children.
void DepartmentTree::printIndented(const DepartmentNode* node, std::size_t depth) {
    std::cout << std::string(depth * 4, ' ') << node->name << '\n';
    for (std::size_t i = 0; i < node->children.size(); ++i) {
        printIndented(node->children[i], depth + 1);
    }
}

// Returns names with each parent before its children.
std::vector<std::string> DepartmentTree::preorder() const {
    std::vector<std::string> names;
    if (root_ != NULL) {
        collectPreorder(root_, names);
    }
    return names;
}

// Visits the node, then each child subtree in insertion order.
void DepartmentTree::collectPreorder(const DepartmentNode* node,
                                     std::vector<std::string>& names) {
    names.push_back(node->name);
    for (std::size_t i = 0; i < node->children.size(); ++i) {
        collectPreorder(node->children[i], names);
    }
}

// Returns names with each parent after its children.
std::vector<std::string> DepartmentTree::postorder() const {
    std::vector<std::string> names;
    if (root_ != NULL) {
        collectPostorder(root_, names);
    }
    return names;
}

// Visits each child subtree in insertion order, then the node.
void DepartmentTree::collectPostorder(const DepartmentNode* node,
                                      std::vector<std::string>& names) {
    for (std::size_t i = 0; i < node->children.size(); ++i) {
        collectPostorder(node->children[i], names);
    }
    names.push_back(node->name);
}

// Visits the tree one depth at a time using a queue.
std::vector<std::string> DepartmentTree::levelOrder() const {
    std::vector<std::string> names;
    if (root_ == NULL) {
        return names;
    }
    std::queue<const DepartmentNode*> pending;
    pending.push(root_);
    while (!pending.empty()) {
        const DepartmentNode* node = pending.front();
        pending.pop();
        names.push_back(node->name);
        for (std::size_t i = 0; i < node->children.size(); ++i) {
            pending.push(node->children[i]);
        }
    }
    return names;
}

// Prints statistics; height is shown as N/A for an empty tree.
void DepartmentTree::printStatistics() const {
    if (root_ == NULL) {
        std::cout << "No departments are loaded.\n"
                  << "Total departments: 0\nLeaf departments: 0\nTree height: N/A\n";
        return;
    }
    std::size_t total = 0;
    std::size_t leaves = 0;
    std::size_t height = 0;
    gatherStatistics(root_, 0, total, leaves, height);
    std::cout << "Total departments: " << total << '\n';
    std::cout << "Leaf departments: " << leaves << '\n';
    std::cout << "Tree height: " << height << '\n';
}

// Updates the running totals for a node and all of its descendants.
void DepartmentTree::gatherStatistics(const DepartmentNode* node, std::size_t depth,
                                      std::size_t& total, std::size_t& leaves,
                                      std::size_t& height) {
    ++total;
    if (node->children.empty()) {
        ++leaves;
    }
    if (depth > height) {
        height = depth;
    }
    for (std::size_t i = 0; i < node->children.size(); ++i) {
        gatherStatistics(node->children[i], depth + 1, total, leaves, height);
    }
}
