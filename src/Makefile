##
# Sneep
#
# @file
# @version 0.1

EXTRA_DEFS = -DTHROW_MANY_ERRS -DQUICK_STATE_OK_CHECK

# Options:
# THROW_ERRS: Throw extra errors/debug statements.
# THROW_MANY_ERRS: Implicitly enables THROW_ERRS, should be obvious.
# [OP] OPTIMIZE_EP_MOVEGEN: Assume ideal conditions for EP movegen. [unstable]
# [OP] QUICK_STATE_OK_CHECK: Don't check all State values.
# ALL_OPTIMIZATIONS: Enable all options with [OP] tags
# NDEBUG: Classically, it disables THROW_ERRS and THROW_MANY_ERRS
# 		  Also just disables `assert` statements as usual.
# MAGICS: Use Magic Bitboards [WIP]

# LIBS = -lgtest

LDFLAGS = -flto -fwrapv
# LDFLAGS += -fsanitize=undefined

CXX = clang++
CXXFLAGS = -ggdb -O2 -std=c++20 -Wall $(EXTRA_DEFS) $(LDFLAGS)

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
