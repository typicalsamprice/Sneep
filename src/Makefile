##
# Sneep
#
# @file
# @version 0.1

CXX = g++
CXXFLAGS = -ggdb -O0 -std=c++17

EXECUTABLE_NAME = sneep.out

CPP_FILES = main.cpp bitboard.cpp
OBJ_FILES = $(CPP_FILES:.cpp=.o)

.PHONY: clean

$(EXECUTABLE_NAME): $(OBJ_FILES)
	$(CXX) -o $@ $^

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -v $(EXECUTABLE_NAME)
	rm -v $(OBJ_FILES)

# end
