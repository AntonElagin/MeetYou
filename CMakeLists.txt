cmake_minimum_required(VERSION 3.10)
project(plus_project)

include_directories(${GMOCK_INCLUDE_DIRS})
set(SOURCES accept_actions.cpp basic_classes.cpp handler_classes.cpp router.cpp validator.cpp chat.cpp)
add_library(st_lib STATIC ${SOURCES})
#include_directories(SYSTEM ${GMOCK_DIR}/include ${GMOCK_DIR}/gtest/include)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
add_executable(tests_gtest test.cpp)
target_link_libraries(tests_gtest PUBLIC ${GTEST_LIBRARIES} pthread st_lib)