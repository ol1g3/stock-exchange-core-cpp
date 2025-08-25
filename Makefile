CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -pthread
INC_FLAGS := -Iinclude
LDFLAGS :=

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)
TARGET := $(BIN_DIR)/exchange

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -c $< -o $@ -MMD -MP

clean:
	@rm -rf $(OBJ_DIR) $(BIN_DIR)

run: all
	./$(TARGET)

-include $(DEPS)