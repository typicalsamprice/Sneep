##
# Sneep
#
# @file
# @version 0.1

EXTRA_DEFS = -DTHROW_MANY_ERRS
LIBS = -lgtest

CXX = clang++
CXXFLAGS = -ggdb -O0 -std=c++20 -Wall $(EXTRA_DEFS) -fsanitize=undefined -fwrapv

LDFLAGS = -flto -fsanitize=undefined

EXECUTABLE_NAME = sneep.out
TEST_EXECUTABLE_NAME = test.out

ALL_CPP_FILES = $(wildcard *.cpp)

CPP_FILES = $(filter-out test.cpp, $(ALL_CPP_FILES))
OBJ_FILES = $(CPP_FILES:.cpp=.o)

CPP_FILES_T = $(filter-out main.cpp, $(ALL_CPP_FILES))
OBJ_FILES_T = $(CPP_FILES_T:.cpp=.o)

.PHONY: clean test lines

$(EXECUTABLE_NAME): $(OBJ_FILES)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS) $(CXXFLAGS)

test: $(TEST_EXECUTABLE_NAME)
$(TEST_EXECUTABLE_NAME): $(OBJ_FILES_T)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS) $(CXXFLAGS)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -vf $(EXECUTABLE_NAME)
	rm -vf $(wildcard *.o)

lines:
	cloc ../**

# end
