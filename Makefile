# Builds the Milestone 1 program by default. "make final" builds the Final menu.
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic -Iinclude
SOURCES = src/main.cpp src/StringUtils.cpp src/HashTable.cpp src/Facility.cpp \
          src/CampusGraph.cpp src/DepartmentTree.cpp src/DataLoader.cpp

campus: $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o campus

final: $(SOURCES)
	$(CXX) $(CXXFLAGS) -DFINAL_MENU $(SOURCES) -o campus

clean:
	rm -f campus campus.exe

.PHONY: final clean
