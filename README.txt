Smart Campus Navigation and Facility Analytics System
=====================================================

PURPOSE
A C++ menu program that loads campus data from text files and lets a user
locate facilities, explore building connections, find routes, browse the
department hierarchy, and view summary reports.

GITHUB REPOSITORY
    https://github.com/gsush701-gif/smart-campus-navigation

TEAM
    Sushil Gautam, Anish Dahal, Vijay Ram Sanvi Bhaskarla

BUILD (run from the project root)
Milestone 1 menu (default):
    g++ -std=c++11 -Wall -Wextra -pedantic -Iinclude src/*.cpp -o campus

Final menu:
    g++ -std=c++11 -Wall -Wextra -pedantic -Iinclude -DFINAL_MENU src/*.cpp -o campus

A Makefile is also provided: "make" builds the Milestone 1 menu, "make final"
builds the Final menu, and "make clean" removes the executable.

RUN (from the project root)
    ./campus                     loads the files in data/
    ./campus testdata/collision  loads the files in another data directory

TEST
Build the Milestone 1 menu as above, then run the [M1] tests:
    bash testdata/run_tests.sh
For the Final menu, build with -DFINAL_MENU and run every test:
    MODE=final bash testdata/run_tests.sh
The script rewrites test_results.txt from the program's real output, and adds
a Valgrind memory check if Valgrind is installed.

DIRECTORY LAYOUT
    include/             Header files
    src/                 Implementation files, including main.cpp
    data/                The four starter input files
    testdata/            Test data directories and run_tests.sh
    README.txt           This file
    README.md            Short project description shown on GitHub
    CONTRIBUTIONS.txt    Team members, module ownership, shared work
    design_document.txt  Design decisions
    test_results.txt     Recorded test results
    Makefile             Optional build commands
    .gitignore           Excludes executables, object files, editor files
    .gitattributes       Preserves line endings in input and test files

TEST DATA DIRECTORIES
    collision        Colliding IDs and a duplicate-key attempt
    collision_nodup  The same IDs without the duplicate, for comparison
    malformed        One record for each rejection rule
    crlf             Windows line endings, extra whitespace, lowercase ID, #
    empty            Four empty files
    missing          Only buildings.txt is present
    rootonly         A root-only tree and a graph with no isolated building
    ties             Equal-length routes, degree ties, several components,
                     and categories that differ by capitalization

MODULE OVERVIEW
    StringUtils     Trimming, case handling, field splitting, ID validation
    HashTable       Custom hash table keyed by facility ID
    Facility        Facility record; FacilityDatabase storage, searches, report
    CampusGraph     Adjacency-list graph, BFS, shortest path, connectivity report
    DepartmentTree  General tree, search, traversals, statistics
    DataLoader      Reads and validates the four input files
    main            Coordinates loading and runs the menu

HASH FUNCTION
Polynomial rolling hash over the uppercase ID. For each character c in order:
    h = (h * 31 + c) mod capacity
The starting value is 0 and the capacity is 101.

COLLISION STRATEGY
Separate chaining. Each bucket is a growable chain, so the table never fills.

GRAPH REPRESENTATION
Adjacency list: one vector of neighbor indexes per building, kept in connection
order. Display, statistics, BFS, shortest path and the connectivity report all
use this one representation.
