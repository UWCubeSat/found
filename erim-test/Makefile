# # Compiler
# CXX = g++
# CXXFLAGS = -std=c++17 -Wall -Wextra -I./src

# # Directories
# SRC_DIR = src
# OBJ_DIR = obj
# BIN_DIR = bin
# DATA_DIR = data

# # Target executable
# TARGET = $(BIN_DIR)/main

# # Source files
# SRCS = $(SRC_DIR)/main.cpp \
#        $(SRC_DIR)/serializer.cpp \
#        $(SRC_DIR)/deserializer.cpp \
#        $(SRC_DIR)/checksum.cpp \
#        $(SRC_DIR)/file_utils.cpp

# # Object files
# OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# # Default target
# all: $(TARGET)

# # Link the object files to create the executable
# $(TARGET): $(OBJS)
# 	@mkdir -p $(BIN_DIR)
# 	$(CXX) $(CXXFLAGS) -o $@ $^

# # Compile source files into object files
# $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
# 	@mkdir -p $(OBJ_DIR)
# 	$(CXX) $(CXXFLAGS) -c $< -o $@

# # Clean up build files
# clean:
# 	rm -rf $(OBJ_DIR) $(BIN_DIR)

# # Run the program
# run: $(TARGET)
# 	./$(TARGET)

# # Ensure the data directory exists
# $(DATA_DIR):
# 	@mkdir -p $(DATA_DIR)

# .PHONY: all clean run

# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./src

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
DATA_DIR = data

# Target executable
TARGET = $(BIN_DIR)/main
TEST_TARGET = $(BIN_DIR)/test_all

# Source files for main program
SRCS = $(SRC_DIR)/main.cpp \
       $(SRC_DIR)/serializer.cpp \
       $(SRC_DIR)/deserializer.cpp \
       $(SRC_DIR)/checksum.cpp \
       $(SRC_DIR)/file_utils.cpp

# Source files for test program
TEST_SRCS = $(SRC_DIR)/test_all.cpp \
            $(SRC_DIR)/serializer.cpp \
            $(SRC_DIR)/deserializer.cpp \
            $(SRC_DIR)/checksum.cpp \
            $(SRC_DIR)/file_utils.cpp

# Object files for main program
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Object files for test program
TEST_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(TEST_SRCS))

# Default target (build main program)
all: $(TARGET)

# Build test program
test: $(TEST_TARGET)

# Link the object files to create the main executable
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Link the object files to create the test executable
$(TEST_TARGET): $(TEST_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Run the main program
run: $(TARGET)
	./$(TARGET)

# Run the test program
run-test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Ensure the data directory exists
$(DATA_DIR):
	@mkdir -p $(DATA_DIR)

.PHONY: all test clean run run-test