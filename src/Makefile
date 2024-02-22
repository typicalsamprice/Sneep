##
# Sneep
#
# @file
# @version 0.1

EXTRA_DEFS = -DTHROW_MANY_ERRS

CXX = clang++
CXXFLAGS = -ggdb -O0 -std=c++20 -Wall $(EXTRA_DEFS)

LDFLAGS = -flto

EXECUTABLE_NAME = sneep.out
TEST_EXECUTABLE_NAME = test.out

ALL_CPP_FILES = $(wildcard *.cpp)

CPP_FILES = $(filter-out test.cpp, $(ALL_CPP_FILES))
OBJ_FILES = $(CPP_FILES:.cpp=.o)

CPP_FILES_T = $(filter-out main.cpp, $(ALL_CPP_FILES))
OBJ_FILES_T = $(CPP_FILES_T:.cpp=.o)

.PHONY: clean test lines

$(EXECUTABLE_NAME): $(OBJ_FILES)
	$(CXX) $(LDFLAGS) -o $@ $^

test: $(TEST_EXECUTABLE_NAME)
$(TEST_EXECUTABLE_NAME): $(OBJ_FILES_T)
	$(CXX) -o $@ $^

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -v $(EXECUTABLE_NAME)
	rm -v $(wildcard *.o)

lines:
	cloc ../**

# end
