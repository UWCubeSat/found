# Define all constant directories
SRC_DIR := src
TEST_DIR := test
LIB_DIR := libraries
BUILD_DIR := build

# Define directory with binaries
BIN_DIR := $(BUILD_DIR)/bin
BIN := $(BIN_DIR)/found
TEST_BIN := $(BIN_DIR)/found-test

# Define directory of external libraries
BUILD_LIBRARY_SRC_DIR := $(BUILD_DIR)/$(LIB_DIR)/$(SRC_DIR)
BUILD_LIBRARY_TEST_DIR := $(BUILD_DIR)/$(LIB_DIR)/$(TEST_DIR)

# Define our directory with all object files
BUILD_ARTIFACTS_DIR := $(BUILD_DIR)/objects
BUILD_SRC_DIR := $(BUILD_ARTIFACTS_DIR)/src
BUILD_TEST_DIR := $(BUILD_ARTIFACTS_DIR)/test

# Define our directory with pre-processed code (for debugging)
BUILD_PRIVATE_DIR := $(BUILD_DIR)/private
BUILD_PRIVATE_SRC_DIR := $(BUILD_PRIVATE_DIR)/$(SRC_DIR)
BUILD_PRIVATE_TEST_DIR := $(BUILD_PRIVATE_DIR)/$(TEST_DIR)

# Define our directory with documentation
BUILD_DOCUMENTATION_DIR := $(BUILD_DIR)/documentation
BUILD_DOCUMENTATION_DOXYGEN_DIR := $(BUILD_DOCUMENTATION_DIR)/doxygen
BUILD_DOCUMENTATION_COVERAGE_DIR := $(BUILD_DOCUMENTATION_DIR)/coverage

# Define the GoogleTest library and build targets
GTEST := googletest
GTEST_VERSION := release-1.12.1
GTEST_URL := https://github.com/google/$(GTEST)/archive/$(GTEST_VERSION).tar.gz
GTEST_DIR := $(BUILD_LIBRARY_TEST_DIR)/$(GTEST)-$(GTEST_VERSION)
GTEST_BUILD_DIR := $(GTEST_DIR)/build

# Define all source and test code
SRC := $(shell find $(SRC_DIR) -name "*.cpp")
SRC_H := $(shell find $(SRC_DIR) -name "*.hpp")
TEST := $(shell find $(TEST_DIR) -name "*.cpp")
TEST_H :=$(shell find $(TEST_DIR) -name "*.hpp")

# Define catch2 library and test suite files
CATCH_LIB := $(TEST_DIR)/catch
TEST_FILES := $(filter-out $(CATCH_LIB)/%, $(TEST) $(TEST_H))

# Our Object source and test files
SRC_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_SRC_DIR)/%.o,$(SRC))
TEST_OBJS := $(patsubst $(TEST_DIR)/%.cpp, $(BUILD_TEST_DIR)/%.o,$(TEST)) $(filter-out %/main.o, $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_TEST_DIR)/%.o, $(SRC)))

# Our pre-processed source and test code
PRIVATE_SRC := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_PRIVATE_SRC_DIR)/%.i,$(SRC))
PRIVATE_TEST := $(patsubst $(TEST_DIR)/%.cpp, $(BUILD_PRIVATE_TEST_DIR)/%.i,$(TEST))

# Out libraries for both source and test suites
SRC_LIBS := -Isrc # Script to automatically include all folders: $(shell find $(SRC_DIR) -type d -print | xargs -I {} echo -I{})
TEST_LIBS := $(SRC_LIBS) -I. # We need to include SRC_LIBS here for the test suite to register all src/**/%.hpp files correctly, but in the test folder, we should be placing the full file path

# Compiler flags
LIBS := $(SRC_LIBS)
LIBS_TEST := -I$(GTEST_DIR)/$(GTEST)/include -I$(GTEST_DIR)/googlemock/include -pthread
DEBUG_FLAGS := -ggdb -fno-omit-frame-pointer
COVERAGE_FLAGS := -fprofile-arcs -ftest-coverage
CXXFLAGS := $(CXXFLAGS) -Ilibraries -Idocumentation -Wall -Wextra -Wno-missing-field-initializers -pedantic --std=c++11 $(LIBS)
CXXFLAGS_TEST := $(CXXFLAGS) $(LIBS_TEST)
LDFLAGS := # Any dynamic libraries go here
LDFLAGS_TEST := $(LDFLAGS) -L$(GTEST_BUILD_DIR)/lib -lgtest -lgtest_main -lgmock -lgmock_main -pthread -lgcov

# Targets
COMPILE_SETUP_TARGET := compile_setup
COMPILE_TARGET := compile
GOOGLE_STYLECHECK_TARGET := google_stylecheck
TEST_SETUP_TARGET := test_setup
TEST_TARGET := test
COVERAGE_TARGET := coverage
GOOGLE_STYLECHECK_TEST_TARGET := google_stylecheck_test
PRIVATE_TARGET := private
DOXYGEN_TARGET := doxygen_generate
CLEAN_TARGET := clean

# Options configurations
ifdef DEBUG
	CXXFLAGS := $(DEBUG_FLAGS) $(CXXFLAGS) 
	CXXFLAGS_TEST := $(DEBUG_FLAGS) $(CXXFLAGS_TEST)
endif
PASS_ON_COVERAGE_FAIL := false

# Prints out a Header when each
# target begins
# 
# Argument
# - $(1) The name of the target
#
# Prints out a banner for each
# target with the specified name
define PRINT_TARGET_HEADER
	@MIDDLE_LINE="Target: $(1)"; \
	MIDDLE_LINE_LEN=$$(echo -n "$$MIDDLE_LINE" | wc -m); \
	EXTRA_CHARS=20; \
	TOTAL_LENGTH=$$((MIDDLE_LINE_LEN + EXTRA_CHARS)); \
	HASH_LINE=$$(printf '%*s' $$TOTAL_LENGTH | tr ' ' '='); \
	printf "\n"; \
	printf "%s\n" "$$HASH_LINE"; \
	printf "          %s\n" "$$MIDDLE_LINE"; \
	printf "%s\n" "$$HASH_LINE"; \
	printf "\n"
endef

# The default target (all)
all: $(COMPILE_SETUP_TARGET) \
	 $(COMPILE_TARGET) \
	 $(GOOGLE_STYLECHECK_TARGET) \
	 $(TEST_TARGET) \
	 $(COVERAGE_TARGET) \
	 $(GOOGLE_STYLECHECK_TEST_TARGET) \
	 $(PRIVATE_TARGET) \
	 $(DOXYGEN_TARGET) \

# The build setup target (sets up appropriate directories)
$(COMPILE_SETUP_TARGET):
	$(call PRINT_TARGET_HEADER, $(COMPILE_SETUP_TARGET))
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)
	mkdir -p $(BUILD_LIBRARY_SRC_DIR)
	mkdir -p $(BUILD_DOCUMENTATION_DIR)

# The compile target
$(COMPILE_TARGET): $(COMPILE_SETUP_TARGET) compile_message $(BIN)
$(BIN): $(SRC_OBJS) $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(BIN) $(SRC_OBJS) $(LDFLAGS)
$(BUILD_SRC_DIR)/%.o: $(SRC_DIR)/%.cpp $(BUILD_DIR) $(BIN_DIR)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(SRC_LIBS)
compile_message:
	$(call PRINT_TARGET_HEADER, $(COMPILE_TARGET))

# The stylecheck target for source code
$(GOOGLE_STYLECHECK_TARGET): $(SRC) $(SRC_H)
	$(call PRINT_TARGET_HEADER, $(GOOGLE_STYLECHECK_TARGET))
	cpplint $(SRC) $(SRC_H)

$(TEST_SETUP_TARGET): $(COMPILE_SETUP_TARGET) test_setup_message $(BUILD_LIBRARY_TEST_DIR) $(GTEST_DIR)
$(BUILD_LIBRARY_TEST_DIR):
	mkdir -p $(BUILD_LIBRARY_TEST_DIR)
	mkdir -p $(BUILD_DOCUMENTATION_COVERAGE_DIR)
test_setup_message:
	$(call PRINT_TARGET_HEADER, $(TEST_SETUP_TARGET))

# The test target
$(TEST_TARGET): $(TEST_SETUP_TARGET) test_message $(TEST_BIN)
	valgrind ./$(TEST_BIN)
$(TEST_BIN): $(GTEST_DIR) $(TEST_OBJS) $(BIN_DIR)
	$(CXX) $(CXXFLAGS_TEST) -o $(TEST_BIN) $(TEST_OBJS) $(LIBS) $(LDFLAGS_TEST)
$(BUILD_TEST_DIR)/%.o: $(TEST_DIR)/%.cpp $(GTEST_DIR) $(BUILD_DIR)
	mkdir -p $(@D)
	$(CXX) $(TEST_LIBS) $(COVERAGE_FLAGS) $(CXXFLAGS_TEST) -c $< -o $@
$(BUILD_TEST_DIR)/%.o: $(SRC_DIR)/%.cpp $(GTEST_DIR) $(BUILD_DIR)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) -c $< -o $@ $(SRC_LIBS)
$(GTEST_DIR): $(BUILD_DIR)
	wget $(GTEST_URL)
	tar -xzf $(GTEST_VERSION).tar.gz -C $(BUILD_LIBRARY_TEST_DIR)
	rm -f $(GTEST_VERSION).tar.gz
	mkdir -p $(GTEST_BUILD_DIR)
	cd $(GTEST_BUILD_DIR) && cmake .. && make
test_message:
	$(call PRINT_TARGET_HEADER, $(TEST_TARGET))

# The coverage target
$(COVERAGE_TARGET): $(TEST_SETUP_TARGET) $(TEST_TARGET)
	$(call PRINT_TARGET_HEADER, $(COVERAGE_TARGET))
	gcovr || $(PASS_ON_COVERAGE_FAIL)

# The stylecheck target for tests
$(GOOGLE_STYLECHECK_TEST_TARGET): $(TEST_FILES)
	$(call PRINT_TARGET_HEADER, $(GOOGLE_STYLECHECK_TEST_TARGET))
	cpplint $(TEST_FILES)

# The pre-processed artifacts target (private)
private: $(COMPILE_SETUP_TARGET) $(TEST_SETUP_TARGET) private_message $(PRIVATE_SRC) $(PRIVATE_TEST)
$(BUILD_PRIVATE_SRC_DIR)/%.i: $(SRC) $(BUILD_DIR)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_TEST) -E $< -o $@ $(SRC_LIBS)
$(BUILD_PRIVATE_TEST_DIR)/%.i: $(TEST) $(BUILD_DIR)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_TEST) -E $< -o $@ $(SRC_LIBS) $(TEST_LIBS)
private_message:
	$(call PRINT_TARGET_HEADER, private)

# The doxygen target
$(DOXYGEN_TARGET): $(COMPILE_SETUP_TARGET)
	$(call PRINT_TARGET_HEADER, $(DOXYGEN_TARGET))
	mkdir -p $(BUILD_DOCUMENTATION_DOXYGEN_DIR)
	doxygen

# The clean target (not in default target)
$(CLEAN_TARGET):
	$(call PRINT_TARGET_HEADER, $(CLEAN_TARGET))
	rm -rf build