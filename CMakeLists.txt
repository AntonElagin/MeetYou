cmake_minimum_required(VERSION 3.10)
project(plus_project)

set(CMAKE_CXX_STANDARD 17)


include_directories(${GMOCK_INCLUDE_DIRS})
set (GMOCK_DIR "/usr/src/googletest")
add_subdirectory(${GMOCK_DIR} ${CMAKE_BINARY_DIR}/googlegmock)
include_directories(SYSTEM ${GMOCK_DIR}/include ${GMOCK_DIR}/gtest/include)
add_executable(tests_gtest test.cpp request_chat_handler.cpp chat.cpp user.cpp message.cpp )
target_link_libraries(tests_gtest PUBLIC ${GTEST_LIBRARIES} pthread gmock_main)