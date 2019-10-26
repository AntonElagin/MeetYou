cmake_minimum_required(VERSION 3.10)
project(plus_project)

set(CMAKE_CXX_STANDARD 17)

add_executable(plus_client main.cpp)
target_link_libraries(plus_client PUBLIC pthread)
add_executable(plus_server server.cpp)
target_link_libraries(plus_server PUBLIC pthread)