##
# Sneep
#
# @file
# @version 0.1

EXTRA_DEFS = -DTHROW_MANY_ERRS

CXX = clang++
CXXFLAGS = -ggdb -O0 -std=c++17 $(EXTRA_DEFS)

LDFLAGS = -flto

EXECUTABLE_NAME = sneep.out

# OR UNCOMMENT THIS LINE INSTEAD:
# CPP_FILES = $(wildcard *.cpp)
CPP_FILES = main.cpp bitboard.cpp position.cpp types.cpp
OBJ_FILES = $(CPP_FILES:.cpp=.o)

.PHONY: clean

$(EXECUTABLE_NAME): $(OBJ_FILES)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -v $(EXECUTABLE_NAME)
	rm -v $(OBJ_FILES)

# end
