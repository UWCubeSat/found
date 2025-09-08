include(FetchContent)
include(ExternalProject)

########## CMake Project Requirements ##########

cmake_minimum_required(VERSION 3.21)
project(found LANGUAGES CXX)

set(ENABLE_WORKFLOW OFF)
if((NOT CMAKE_BUILD_TYPE STREQUAL "Release") AND PROJECT_IS_TOP_LEVEL)
    set(ENABLE_WORKFLOW ON)
endif()

########## Utilities ##########

# Checks if the program exists and has correct version #
function(check_program_version out_var program_name required_version)
    # Find the program
    find_program(PROGRAM_PATH_${out_var} ${program_name})
    if(NOT PROGRAM_PATH_${out_var})
        message(FATAL_ERROR "${program_name} not found")
    endif()

    # Get the version string (adjust the flag if needed)
    execute_process(
        COMMAND ${PROGRAM_PATH_${out_var}} --version
        OUTPUT_VARIABLE VERSION_RAW
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    # Extract the version number (first x.y or x.y.z match)
    string(REGEX MATCH "[0-9]+\\.[0-9]+(\\.[0-9]+)?" VERSION_EXTRACTED "${VERSION_RAW}")
    if(VERSION_EXTRACTED VERSION_LESS ${required_version})
        message(
            FATAL_ERROR
                "${program_name} version ${required_version} or higher required, but found ${VERSION_EXTRACTED}"
        )
    endif()

    # Return both the path and version
    set(${out_var}
        ${PROGRAM_PATH_${out_var}}
        PARENT_SCOPE)
endfunction()

########## File/Folder Definitions ##########

# Build directories
set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(CACHE_DIR ${PROJECT_SOURCE_DIR}/.cache)
set(BIN_DIR ${BUILD_DIR}/bin)
# set(OBJ_DIR ${BUILD_DIR}/objects) No longer needed
set(DOC_DIR ${BUILD_DIR}/documentation)

# Source directory
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
file(GLOB_RECURSE SRC ${SRC_DIR}/*.cpp)
file(GLOB_RECURSE SRC_H ${SRC_DIR}/*.hpp)
set(FILTERED_SRC ${SRC})
list(FILTER FILTERED_SRC EXCLUDE REGEX ".*src/main\\.cpp$")
set(SRC_MAIN "${SRC_DIR}/main.cpp")

# Test directory
set(TEST_DIR ${PROJECT_SOURCE_DIR}/test)
file(GLOB_RECURSE TEST CONFIGURE_DEPENDS ${TEST_DIR}/*.cpp)
file(GLOB_RECURSE TEST_H ${TEST_DIR}/*.hpp)
list(APPEND TEST_H ${SRC_DIR})

# Documentation directory
set(DOC_COVERAGE_DIR ${DOC_DIR}/coverage)
set(DOC_DOXYGEN_DIR ${DOC_DIR}/doxygen)

########## C++ Shared Compilation Flags ##########

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include dirs
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${SRC_DIR})

# Logging macros
option(DISABLE_LOGGING "Disable logging" OFF)
set(LOGGING_LEVEL
    INFO
    CACHE STRING "Logging level")

# Floating-point config
option(FLOAT_MODE "Enable FOUND_FLOAT_MODE" OFF)
if(FLOAT_MODE)
    add_compile_definitions(FOUND_FLOAT_MODE)
endif()

# Debug config
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -fno-omit-frame-pointer")

# Release config
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Global compile options
add_compile_options(-Wall -Wextra -Wno-missing-field-initializers -pedantic)

########## Source Libraries ##########

# We compile all libraries as static libraries

##### stb_image #####

# stb_image definitions
set(STB_IMAGE "stb_image")
set(STB_IMAGE_URL
    "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h")
set(STB_IMAGE_DIR "${CACHE_DIR}/${STB_IMAGE}")
set(STB_IMAGE_HEADER "${STB_IMAGE_DIR}/${STB_IMAGE}.h")
set(STB_IMAGE_SRC "${STB_IMAGE_DIR}/${STB_IMAGE}.cpp")

# Setup directory for content
file(MAKE_DIRECTORY ${STB_IMAGE_DIR})
FetchContent_Declare(
    ${STB_IMAGE}
    URL https://raw.githubusercontent.com/nothings/stb/master/${STB_IMAGE}.h
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# Download stb_image and compile into library
if(NOT EXISTS "${STB_IMAGE_HEADER}")
    message(STATUS "Downloading ${STB_IMAGE}")
    file(DOWNLOAD ${STB_IMAGE_URL} ${STB_IMAGE_HEADER})
endif()
if(NOT EXISTS ${STB_IMAGE_SRC})
    file(WRITE ${STB_IMAGE_SRC} "#include \"${STB_IMAGE}/${STB_IMAGE}.h\"\n")
endif()
add_library(${STB_IMAGE} STATIC ${STB_IMAGE_SRC})
target_compile_definitions(${STB_IMAGE} PRIVATE STB_IMAGE_IMPLEMENTATION)
target_include_directories(${STB_IMAGE} PUBLIC $<BUILD_INTERFACE:${CACHE_DIR}>)

# Set the source libraries
set(SRC_LIBS ${STB_IMAGE})

##### FOUND Library #####

add_library(found_lib STATIC ${FILTERED_SRC})
target_link_libraries(found_lib PRIVATE ${STB_IMAGE})
target_include_directories(found_lib
        PUBLIC
        $<BUILD_INTERFACE:${SRC_DIR}>
        $<INSTALL_INTERFACE:include>)
target_compile_definitions(found_lib PRIVATE 
        $<$<NOT:$<BOOL:${DISABLE_LOGGING}>>:ENABLE_LOGGING>
        $<$<NOT:$<BOOL:${DISABLE_LOGGING}>>:LOGGING_LEVEL=${LOGGING_LEVEL}>)
add_library(found::found_lib ALIAS found_lib)

########## Test Libraries ##########

if(ENABLE_WORKFLOW)

##### Googletest #####

# We lose the cache here, but I'll take that tradeoff for ease here
set(GTEST googletest)
set(GTEST_VERSION release-1.12.1)
set(GTEST_LIBS gmock gmock_main)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/${GTEST}.git
    GIT_TAG ${GTEST_VERSION}
)

FetchContent_MakeAvailable(googletest)

set(TEST_LIBS ${GTEST_LIBS} ${SRC_LIBS})

##### Documentation Libraries #####

set(DOXYGEN_AWESOME doxygen-awesome-css)
set(DOXYGEN_AWESOME_VERSION 2.3.4)
set(DOXYGEN_AWESOME_URL
    https://github.com/jothepro/${DOXYGEN_AWESOME}/archive/refs/tags/v${DOXYGEN_AWESOME_VERSION}.tar.gz
)
set(DOXYGEN_AWESOME_ZIP ${CACHE_DIR}/v${DOXYGEN_AWESOME_VERSION}.tar.gz)
set(DOXYGEN_AWESOME_ARTIFACT
    ${CACHE_DIR}/${DOXYGEN_AWESOME}-${DOXYGEN_AWESOME_VERSION})

add_custom_command(
    OUTPUT ${DOXYGEN_AWESOME_ARTIFACT}
    COMMAND wget ${DOXYGEN_AWESOME_URL} -P ${CACHE_DIR}
    COMMAND tar -xzf ${DOXYGEN_AWESOME_ZIP} -C ${CACHE_DIR}
    VERBATIM
    COMMENT "Downloads doxygen-awesome")

endif()

########## Targets ##########

add_compile_options(-Wdouble-promotion -Werror)

##### compile #####

file(MAKE_DIRECTORY ${BIN_DIR})

add_executable(found ${SRC_MAIN})

target_link_libraries(found PRIVATE ${SRC_LIBS} found_lib)
target_compile_definitions(found PRIVATE 
        $<$<NOT:$<BOOL:${DISABLE_LOGGING}>>:ENABLE_LOGGING>
        $<$<NOT:$<BOOL:${DISABLE_LOGGING}>>:LOGGING_LEVEL=${LOGGING_LEVEL}>)
set_target_properties(found PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
add_custom_target(compile
                  DEPENDS found
                  COMMENT "Compiles source code")

if(ENABLE_WORKFLOW)

##### test #####

add_executable(found-test EXCLUDE_FROM_ALL ${TEST} ${FILTERED_SRC})
target_compile_definitions(found-test PRIVATE ENABLE_LOGGING
                                                LOGGING_LEVEL=INFO)
target_compile_options(found-test PRIVATE
    --coverage
    -pthread
    $<$<NOT:$<BOOL:${OMIT_ASAN}>>:-fsanitize=address>
    $<$<NOT:$<BOOL:${OMIT_ASAN}>>:-fomit-frame-pointer>
)
target_link_libraries(found-test PRIVATE
    --coverage
    ${TEST_LIBS}
    $<$<NOT:$<BOOL:${OMIT_ASAN}>>:-fsanitize=address>
    $<$<NOT:$<BOOL:${OMIT_ASAN}>>:-fomit-frame-pointer>)
set_target_properties(found-test PROPERTIES
                        RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
add_dependencies(found-test ${TEST_LIBS})
add_custom_target(test
                  DEPENDS found-test
                  COMMENT "Compiles test code")

##### coverage #####

file(MAKE_DIRECTORY ${DOC_COVERAGE_DIR})

check_program_version(GCOVR gcovr 8.3)
set(GCOVR_CONFIG ${PROJECT_SOURCE_DIR}/gcovr.cfg)

add_custom_target(
    coverage
    COMMAND cd ${PROJECT_SOURCE_DIR} && ${BIN_DIR}/found-test --gtest_brief=1
    COMMAND ${GCOVR} -r ${PROJECT_SOURCE_DIR}
    # WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Running tests and generating coverage report"
    VERBATIM
    DEPENDS found-test)

##### linting #####

check_program_version(CPPLINT cpplint 2.0.0)

add_custom_target(
    lint
    COMMAND ${CPPLINT} --recursive ${SRC_DIR} ${TEST_DIR}
    DEPENDS ${SRC} ${TEST}
    COMMENT "Checks Google C++ Style on Code")

##### documentation #####

find_package(Doxygen 1.9.8 REQUIRED)
if(NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is required but was not found.")
endif()

add_custom_target(
    documentation
    COMMAND doxygen
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    VERBATIM
    DEPENDS ${DOXYGEN_AWESOME_ARTIFACT} ${SRC}
    COMMENT "Generates Doxygen Documentation over code")

endif()

##### clean_all #####

add_custom_target(clean_all
                  COMMAND rm -rf ${CACHE_DIR} ${BUILD_DIR}
                  COMMENT "Cleans the build and cache folders")

##### default target (does not run in order) #####
add_custom_target(default ALL
                  COMMENT "The default target")

if(ENABLE_WORKFLOW)
    add_dependencies(default documentation)
    add_dependencies(documentation lint)
    add_dependencies(lint coverage)
    add_dependencies(coverage test)
    add_dependencies(test compile)
else()
    add_dependencies(default compile)
endif()

install(TARGETS found DESTINATION bin)
install(TARGETS found_lib
        EXPORT foundTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include)
install(TARGETS ${STB_IMAGE}
        EXPORT foundTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        INCLUDES DESTINATION include)
install(EXPORT foundTargets
        NAMESPACE found::
        DESTINATION lib/cmake/found)
install(DIRECTORY ${SRC_DIR}/
        DESTINATION include
        FILES_MATCHING
        PATTERN "*.hpp")
