# Define all constant directories
SRC_DIR := src
TEST_DIR := test
LIB_DIR := libraries
BUILD_DIR := build
CACHE_DIR := .cache

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

# Define the stb_image library
STB_IMAGE := stb_image
STB_IMAGE_URL := https://raw.githubusercontent.com/nothings/stb/master/$(STB_IMAGE).h
STB_IMAGE_CACHE_DIR := $(CACHE_DIR)/$(STB_IMAGE)
STB_IMAGE_CACHE_ARTIFACT := $(STB_IMAGE_CACHE_DIR)/$(STB_IMAGE).cpp
STB_IMAGE_DIR := $(BUILD_LIBRARY_SRC_DIR)/$(STB_IMAGE)

# Define the GoogleTest library and build targets
GTEST := googletest
GTEST_VERSION := release-1.12.1
GTEST_URL := https://github.com/google/$(GTEST)/archive/$(GTEST_VERSION).tar.gz
GTEST_CACHE_ARTIFACT := $(CACHE_DIR)/$(GTEST_VERSION).tar.gz
GTEST_CACHE_DIR := $(CACHE_DIR)/$(GTEST)-$(GTEST_VERSION)
GTEST_CACHE_BUILD_DIR := $(GTEST_CACHE_DIR)/build
GTEST_DIR := $(BUILD_LIBRARY_TEST_DIR)/$(GTEST)-$(GTEST_VERSION)

# Define the Doxygen style library
DOXYGEN_AWESOME = doxygen-awesome-css
DOXYGEN_AWESOME_VERSION := 2.3.4
DOXYGEN_AWESOME_URL := https://github.com/jothepro/$(DOXYGEN_AWESOME)/archive/refs/tags/v$(DOXYGEN_AWESOME_VERSION).tar.gz
DOXYGEN_AWESOME_ZIP := $(CACHE_DIR)/v$(DOXYGEN_AWESOME_VERSION).tar.gz
DOXYGEN_AWESOME_ARTIFACT := $(CACHE_DIR)/$(DOXYGEN_AWESOME)-$(DOXYGEN_AWESOME_VERSION)


# Define all source and test code
SRC := $(shell find $(SRC_DIR) -name "*.cpp")
SRC_H := $(shell find $(SRC_DIR) -name "*.hpp")
TEST := $(shell find $(TEST_DIR) -name "*.cpp")
TEST_H :=$(shell find $(TEST_DIR) -name "*.hpp")

# Our Object source and test files
SRC_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_SRC_DIR)/%.o,$(SRC))
TEST_OBJS := $(patsubst $(TEST_DIR)/%.cpp, $(BUILD_TEST_DIR)/%.o,$(TEST)) $(filter-out %/main.o, $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_TEST_DIR)/%.o, $(SRC)))

# Our pre-processed source and test code
PRIVATE_SRC := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_PRIVATE_SRC_DIR)/%.i,$(SRC))
PRIVATE_TEST := $(patsubst $(TEST_DIR)/%.cpp, $(BUILD_PRIVATE_TEST_DIR)/%.i,$(TEST))

# Out libraries for both source and test suites
SRC_LIBS := -Isrc # Script to automatically include all folders: $(shell find $(SRC_DIR) -type d -print | xargs -I {} echo -I{})
TEST_LIBS := $(SRC_LIBS) -I. # We need to include SRC_LIBS here for the test suite to register all src/**/%.hpp files correctly, but in the test folder, we should be placing the full file path

# DISABLE_LOGGING can be defined to disable logging on ./found
ifndef DISABLE_LOGGING
	ifdef LOGGING_LEVEL
		LOGGING_MACROS := -DENABLE_LOGGING -DLOGGING_LEVEL=$(LOGGING_LEVEL)
  	else
    	LOGGING_MACROS := -DENABLE_LOGGING -DLOGGING_LEVEL=INFO
  	endif
endif

# FLOAT_MODE can be defined to enable FOUND_FLOAT_MODE
ifdef FLOAT_MODE
	FOUND_FLOAT_MODE_MACRO := -DFOUND_FLOAT_MODE -Wdouble-promotion -Werror=double-promotion
endif

LOGGING_MACROS_TEST := -DENABLE_LOGGING -DLOGGING_LEVEL=INFO -DINFO_STREAM=std::cout -DWARN_STREAM=std::cerr -DERROR_STREAM=std::cerr

# Compiler flags
LIBS := $(SRC_LIBS) -I$(BUILD_LIBRARY_SRC_DIR)
LIBS_TEST := -I$(GTEST_DIR)/$(GTEST)/include -I$(GTEST_DIR)/googlemock/include -pthread
DEBUG_FLAGS := -ggdb -fno-omit-frame-pointer
COVERAGE_FLAGS := --coverage
CXXFLAGS := $(CXXFLAGS) -Wall -Wextra -Wno-missing-field-initializers -pedantic --std=c++17 -MMD $(LIBS) $(FOUND_FLOAT_MODE_MACRO)
CXXFLAGS_TEST := $(CXXFLAGS) $(LIBS_TEST) $(LOGGING_MACROS_TEST)
ifndef OMIT_ASAN
	CXXFLAGS_TEST := $(CXXFLAGS_TEST) -fsanitize=address -fomit-frame-pointer # Also allow light optimization to get rid of dead code
endif
CXXFLAGS += $(LOGGING_MACROS)
LDFLAGS := $(STB_IMAGE_DIR)/$(STB_IMAGE).o # Any external libraries go here
LDFLAGS_TEST := $(LDFLAGS) -L$(GTEST_CACHE_BUILD_DIR)/lib -lgtest -lgtest_main -lgmock -lgmock_main -pthread

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
CLEAN_ALL_TARGET := clean_all

# DEBUG can be defined to enable debugging on all binaries
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
	 $(TEST_SETUP_TARGET) \
	 $(TEST_TARGET) \
	 $(COVERAGE_TARGET) \
	 $(GOOGLE_STYLECHECK_TEST_TARGET) \
	 $(PRIVATE_TARGET) \
	 $(DOXYGEN_TARGET) \

# The build setup target (sets up appropriate directories)
$(COMPILE_SETUP_TARGET): compile_setup_message $(BUILD_DIR) $(STB_IMAGE_DIR)
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DOCUMENTATION_DIR)
	mkdir -p $(STB_IMAGE_CACHE_DIR)
	mkdir -p $(CACHE_DIR)
$(BIN_DIR):
	mkdir -p $(BIN_DIR)
$(BUILD_LIBRARY_SRC_DIR):
	mkdir -p $(BUILD_LIBRARY_SRC_DIR)
compile_setup_message:
	$(call PRINT_TARGET_HEADER, $(COMPILE_SETUP_TARGET))
$(STB_IMAGE_DIR): $(STB_IMAGE_CACHE_ARTIFACT) $(BUILD_LIBRARY_SRC_DIR)
	cp -r $(STB_IMAGE_CACHE_DIR) $(BUILD_LIBRARY_SRC_DIR)
$(STB_IMAGE_CACHE_ARTIFACT):
	wget $(STB_IMAGE_URL) -P $(STB_IMAGE_CACHE_DIR)
	echo '#define STB_IMAGE_IMPLEMENTATION\n#include "stb_image/stb_image.h"' > $(STB_IMAGE_CACHE_ARTIFACT)
	$(CXX) $(CXXFLAGS) -I$(CACHE_DIR) -c $(STB_IMAGE_CACHE_ARTIFACT) -o $(STB_IMAGE_CACHE_DIR)/$(STB_IMAGE).o

# The compile target
$(COMPILE_TARGET): $(COMPILE_SETUP_TARGET) compile_message $(BIN)
$(BIN): $(SRC_OBJS) $(BIN_DIR) $(STB_IMAGE_DIR)
	$(CXX) $(OPTIMIZATION) $(CXXFLAGS) -o $(BIN) $(SRC_OBJS) $(LDFLAGS)
$(BUILD_SRC_DIR)/%.o: $(SRC_DIR)/%.cpp $(STB_IMAGE_DIR)
	mkdir -p $(@D)
	$(CXX) $(OPTIMIZATION) $(CXXFLAGS) -c $< -o $@
compile_message:
	$(call PRINT_TARGET_HEADER, $(COMPILE_TARGET))

# The stylecheck target for source code
$(GOOGLE_STYLECHECK_TARGET): $(SRC) $(SRC_H)
	$(call PRINT_TARGET_HEADER, $(GOOGLE_STYLECHECK_TARGET))
	cpplint $(SRC) $(SRC_H)

# The test setup target (sets up directories and gtest)
$(TEST_SETUP_TARGET): $(COMPILE_SETUP_TARGET) test_setup_message $(BUILD_DOCUMENTATION_COVERAGE_DIR) $(GTEST_DIR)
$(BUILD_DOCUMENTATION_COVERAGE_DIR):
	mkdir -p $(BUILD_DOCUMENTATION_COVERAGE_DIR)
$(GTEST_DIR): $(GTEST_CACHE_DIR)
	mkdir -p $(BUILD_LIBRARY_TEST_DIR)
	cp -r $(GTEST_CACHE_DIR) $(BUILD_LIBRARY_TEST_DIR)
$(GTEST_CACHE_DIR):
	wget $(GTEST_URL) -P $(CACHE_DIR)
	tar -xzf $(GTEST_CACHE_ARTIFACT) -C $(CACHE_DIR)
	mkdir -p $(GTEST_CACHE_BUILD_DIR)
	cd $(GTEST_CACHE_BUILD_DIR) && cmake .. && make -j16
	rm $(GTEST_CACHE_ARTIFACT)
test_setup_message:
	$(call PRINT_TARGET_HEADER, $(TEST_SETUP_TARGET))

# The test target
$(TEST_TARGET): $(TEST_SETUP_TARGET) test_message $(TEST_BIN)
$(TEST_BIN): $(GTEST_DIR) $(TEST_OBJS) $(BIN_DIR)
	$(CXX) $(CXXFLAGS_TEST) $(COVERAGE_FLAGS) -o $(TEST_BIN) $(TEST_OBJS) $(LIBS) $(LDFLAGS_TEST)
$(BUILD_TEST_DIR)/%.o: $(TEST_DIR)/%.cpp $(GTEST_DIR)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_TEST) $(TEST_LIBS) $(COVERAGE_FLAGS) -c $< -o $@
$(BUILD_TEST_DIR)/%.o: $(SRC_DIR)/%.cpp $(GTEST_DIR)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_TEST) $(COVERAGE_FLAGS) -c $< -o $@ $(SRC_LIBS)
test_message:
	$(call PRINT_TARGET_HEADER, $(TEST_TARGET))

# The coverage target
$(COVERAGE_TARGET): $(TEST_SETUP_TARGET) $(TEST_TARGET)
	$(call PRINT_TARGET_HEADER, $(COVERAGE_TARGET))
	./$(TEST_BIN)
	gcovr || $(PASS_ON_COVERAGE_FAIL)

# The stylecheck target for tests
$(GOOGLE_STYLECHECK_TEST_TARGET): $(TEST) $(TEST_H)
	$(call PRINT_TARGET_HEADER, $(GOOGLE_STYLECHECK_TEST_TARGET))
	cpplint $(TEST) $(TEST_H)

# The pre-processed artifacts target (private)
$(PRIVATE_TARGET): $(COMPILE_SETUP_TARGET) $(TEST_SETUP_TARGET) private_message $(PRIVATE_SRC) $(PRIVATE_TEST)
$(BUILD_PRIVATE_SRC_DIR)/%.i: $(SRC)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -E $< -o $@ $(SRC_LIBS)
$(BUILD_PRIVATE_TEST_DIR)/%.i: $(TEST)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_TEST) -E $< -o $@ $(SRC_LIBS) $(TEST_LIBS)
private_message:
	$(call PRINT_TARGET_HEADER, private)

# The doxygen target	
$(DOXYGEN_AWESOME_ARTIFACT):
	wget $(DOXYGEN_AWESOME_URL) -P $(CACHE_DIR)
	tar -xzf $(DOXYGEN_AWESOME_ZIP) -C $(CACHE_DIR)
$(DOXYGEN_TARGET): $(COMPILE_SETUP_TARGET) $(DOXYGEN_AWESOME_ARTIFACT)
	$(call PRINT_TARGET_HEADER, $(DOXYGEN_TARGET))
	mkdir -p $(BUILD_DOCUMENTATION_DOXYGEN_DIR)
	chmod +rwx doxygen.sh
	./doxygen.sh

# The clean target (not in default target, cleans just the build folder)
$(CLEAN_TARGET):
	$(call PRINT_TARGET_HEADER, $(CLEAN_TARGET))
	rm -rf $(BUILD_DIR)

# The clean_all target (cleans the build and cache folders)
$(CLEAN_ALL_TARGET):
	$(call PRINT_TARGET_HEADER, $(CLEAN_ALL_TARGET))
	rm -rf $(BUILD_DIR) $(CACHE_DIR)

# The release target
release: OPTIMIZATION = -O3
release: CXXFLAGS += -DNDEBUG
release: compile

-include $(SRC_OBJS:.o=.d) $(TEST_OBJS:.o=.d)