cmake_minimum_required(VERSION 3.10)
project(plus_project)

set(CMAKE_CXX_STANDARD 17)

add_executable(plus_client chat.cpp request_chat_handler.cpp user.cpp message.cpp)
target_link_libraries(plus_client PUBLIC pthread)
find_package(GTest REQUIRED )
find_package(Threads REQUIRED )

include_directories(${GTEST_INCLUDE_DIRS})
add_executable( tests_gtest test.cpp request_chat_handler.cpp chat.cpp user.cpp)
target_link_libraries(tests_gtest PUBLIC ${GTEST_LIBRARIES} pthread)