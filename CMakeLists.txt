cmake_minimum_required(VERSION 3.10)
project(print)
set(PRINT_VERSION_MAJOR 0)
set(PRINT_VERSION_MINOR 1)
set(PRINT_VERSION_PATCH 0)
set(PRINT_VERSION_TWEAK 0)
set(PRINT_VERSION
  ${PRINT_VERSION_MAJOR}.${PRINT_VERSION_MINOR}.${PRINT_VERSION_PATCH}.${PRINT_VERSION_TWEAK})
set(PRINT_VERSION_STRING "v${PRINT_VERSION}")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_TESTS "Build tests" OFF)

add_library(print STATIC sources/print.cpp)
target_include_directories(print PUBLIC include)

if(BUILD_TESTS)
  enable_testing()
  
  include(FetchContent)
  FetchContent_Declare(googletest GIT_REPOSITORY https://github.com/google/googletest.git GIT_TAG v1.14.0)
  FetchContent_MakeAvailable(googletest)
  
  add_executable(check tests/test1.cpp)
  target_link_libraries(check PRIVATE print gtest_main)
  target_include_directories(check PRIVATE include)
  add_test(NAME check COMMAND check)
endif()
include(CPackConfig.cmake)
