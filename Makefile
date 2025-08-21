CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Iinclude
LDFLAGS :=

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
TARGET := $(BIN_DIR)/exchange

.PHONY: all clean

all: directories $(TARGET)

directories:
    mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
    $(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
    $(CXX) $^ -o $@ $(LDFLAGS)

clean:
    rm -rf $(OBJ_DIR) $(BIN_DIR)

run: $(TARGET)
    ./$(TARGET)