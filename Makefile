CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -pthread
PERF_CXXFLAGS := -std=c++20 -Wall -Wextra -pthread -O3 -DNDEBUG
INC_FLAGS := -Iinclude
LDFLAGS :=

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
PERF_DIR := tests/performance

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)
TARGET := $(BIN_DIR)/exchange

LIB_OBJS := $(filter-out $(OBJ_DIR)/main.o, $(OBJS))

PERF_SRCS := $(shell find $(PERF_DIR) -name '*.cpp' 2>/dev/null)
PERF_OBJS := $(patsubst $(PERF_DIR)/%.cpp,$(OBJ_DIR)/$(PERF_DIR)/%.o,$(PERF_SRCS))
PERF_DEPS := $(PERF_OBJS:.o=.d)
PERF_TARGET := $(BIN_DIR)/perf_runner

BREW_PREFIX := $(shell brew --prefix)

.PHONY: all clean run perf

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -c $< -o $@ -MMD -MP

$(OBJ_DIR)/$(TEST_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) $(GTEST_INC) -c $< -o $@ -MMD -MP

$(OBJ_DIR)/$(PERF_DIR)/%.o: $(PERF_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(PERF_CXXFLAGS) $(INC_FLAGS) -c $< -o $@ -MMD -MP

$(PERF_TARGET): $(LIB_OBJS) $(PERF_OBJS)
	@mkdir -p $(@D)
	$(CXX) $(PERF_CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	@rm -rf $(OBJ_DIR) $(BIN_DIR)

run: all
	./$(TARGET)

perf: $(PERF_TARGET)
	@echo "Running performance tests..."
	./$(PERF_TARGET) --perf-only

-include $(DEPS)
-include $(PERF_DEPS)