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

.PHONY: build clean run perf
all: clean build run perf

build: $(TARGET) $(PERF_TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Built exchange binary: $@"

$(PERF_TARGET): $(LIB_OBJS) $(PERF_OBJS)
	@mkdir -p $(@D)
	$(CXX) $(PERF_CXXFLAGS) $^ -o $@
	@echo "Built performance test binary: $@"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -c $< -o $@ -MMD -MP

$(OBJ_DIR)/$(PERF_DIR)/%.o: $(PERF_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(PERF_CXXFLAGS) $(INC_FLAGS) -c $< -o $@ -MMD -MP

run: $(TARGET)
	./$(TARGET)

perf: $(PERF_TARGET)
	@echo "Running performance tests..."
	./$(PERF_TARGET) --perf-only

clean:
	@rm -rf $(OBJ_DIR) $(BIN_DIR)

-include $(DEPS)
-include $(PERF_DEPS)