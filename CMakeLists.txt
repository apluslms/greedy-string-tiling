# You don't need this file for installing the Python extension as
# its build process is handled by setuptools.
# This file exists for development purposes, e.g. compiling the unit tests
cmake_minimum_required(VERSION 3.5)
project(GreedyStringTiling)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -O2")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # for editor auto completer

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#set(THIRD_PARTY_DIR $ENV{HOME}/vendor)
if(NOT THIRD_PARTY_DIR)
    message(FATAL_ERROR "Directory for third party dependencies not set, cannot compile.")
endif()

set(CATCH2_HEADER_DIR ${THIRD_PARTY_DIR}/Catch2/single_include)
set(ROLLINGHASH_INCLUDES ${THIRD_PARTY_DIR}/rollinghashcpp)

add_library(Matcher src/gst.cpp)

find_program(CLANG_TIDY_BIN NAMES "clang-tidy")
if(NOT CLANG_TIDY_BIN)
    message(WARNING "Could not find clang-tidy, no linting available.")
else()
    set(DO_CLANG_TIDY ${CLANG_TIDY_BIN} "-checks=*")
endif()

set(TESTS_EXECUTABLE run_tests)
set(TESTS_SOURCES tests/test_matcher.cpp)

set(BENCHMARK_EXECUTABLE run_benchmark)
set(BENCHMARK_SOURCES tests/test_benchmark.cpp)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_executable(${TESTS_EXECUTABLE} ${TESTS_SOURCES})
add_executable(${BENCHMARK_EXECUTABLE} ${BENCHMARK_SOURCES})

target_link_libraries(${TESTS_EXECUTABLE} Matcher)
target_link_libraries(${BENCHMARK_EXECUTABLE}
    Threads::Threads
    Matcher)

target_include_directories(Matcher PRIVATE include ${ROLLINGHASH_INCLUDES})
target_include_directories(${TESTS_EXECUTABLE} PRIVATE
    ${CATCH2_HEADER_DIR}
    include)
target_include_directories(${BENCHMARK_EXECUTABLE} PRIVATE include)

if(CLANG_TIDY_BIN)
    set_target_properties(Matcher ${TESTS_EXECUTABLE} ${BENCHMARK_EXECUTABLE}
        PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
endif()

