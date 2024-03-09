CXX = g++
FLAGS = -std=c++17
CFLAGS = -c
DEBUGFLAGS = -g

SRCPATH = ./src
BOOSTPATH = "" # ! Remember to specify the path

all: global_floorplan
debug: global_floorplan_debug
verify: ./utils/verify

# LINKFLAGS = -pedantic -Wall -fomit-frame-pointer -funroll-all-loops -O3
LINKFLAGS = -O3

global_floorplan: main.o parser.o globmodule.o rgsolver.o
	$(CXX) $(FLAGS) -I $(BOOSTPATH) $(LINKFLAGS) $^ -o $@

main.o: $(SRCPATH)/main.cpp 
	$(CXX) $(FLAGS) -I $(BOOSTPATH) $(CFLAGS) $(LINKFLAGS) $< -o $@

%.o: $(SRCPATH)/%.cpp $(SRCPATH)/%.h
	$(CXX) $(FLAGS) -I $(BOOSTPATH) $(CFLAGS) $(LINKFLAGS) $< -o $@



global_floorplan_debug: main_db.o parser_db.o globmodule_db.o rgsolver_db.o
	$(CXX) $(DEBUGFLAGS) -I $(BOOSTPATH) $(LINKFLAGS) $^ -o $@

main_db.o: $(SRCPATH)/main.cpp 
	$(CXX) $(DEBUGFLAGS) -I $(BOOSTPATH) $(CFLAGS) $< -o $@

%_db.o: $(SRCPATH)/%.cpp $(SRCPATH)/%.h
	$(CXX) $(DEBUGFLAGS) -I $(BOOSTPATH) $(CFLAGS) $< -o $@


clean:
	rm -rf *.o *.out global_floorplan
