CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -pthread
PERF_CXXFLAGS := $(CXXFLAGS) -O3 -DNDEBUG
INC_FLAGS := -Iinclude

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
PERF_DIR := tests/performance

TARGET := $(BIN_DIR)/exchange
SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

LIB_OBJS := $(filter-out $(OBJ_DIR)/main.o, $(OBJS))

PERF_TARGET := $(BIN_DIR)/perf_runner
PERF_SRCS := $(shell find $(PERF_DIR) -name '*.cpp' 2>/dev/null)
PERF_OBJS := $(patsubst $(PERF_DIR)/%.cpp,$(OBJ_DIR)/$(PERF_DIR)/%.o,$(PERF_SRCS))
PERF_DEPS := $(PERF_OBJS:.o=.d)

TEST_DIR := tests/unit
GTEST_LIBS := -lgtest -lgtest_main -pthread

TEST_SRCS := $(shell find $(TEST_DIR) -name '*.cpp')
TEST_OBJS := $(patsubst $(TEST_DIR)/%.cpp,$(OBJ_DIR)/$(TEST_DIR)/%.o,$(TEST_SRCS))
TEST_TARGET := $(BIN_DIR)/unit_tests

.PHONY: all build clean run perf test

all: clean build run test

build: $(TARGET) $(PERF_TARGET) $(TEST_TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Built exchange binary: $@"

$(PERF_TARGET): $(LIB_OBJS) $(PERF_OBJS)
	@mkdir -p $(@D)
	$(CXX) $(PERF_CXXFLAGS) $^ -o $@
	@echo "Built performance test binary: $@"

$(TEST_TARGET): $(LIB_OBJS) $(TEST_OBJS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(GTEST_LIBS)
	@echo "Built unit test binary: $@"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -c $< -o $@ -MMD -MP

$(OBJ_DIR)/$(PERF_DIR)/%.o: $(PERF_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(PERF_CXXFLAGS) $(INC_FLAGS) -c $< -o $@ -MMD -MP

$(OBJ_DIR)/$(TEST_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -c $< -o $@ -MMD -MP

run: $(TARGET)
	./$(TARGET)

perf: $(PERF_TARGET)
	@echo "Running performance tests..."
	./$(PERF_TARGET)

test: $(TEST_TARGET)
	@echo "Running unit tests..."
	./$(TEST_TARGET)

clean:
	@rm -rf $(OBJ_DIR) $(BIN_DIR)

-include $(DEPS)
-include $(PERF_DEPS)