cmake_minimum_required(VERSION 3.10)
project(techProject)

set(CMAKE_CXX_STANDARD 11)

set(BOOST_INCLUDEDIR "/home/anton/apps/boost_1_71_0")
set(BOOST_LIBRARYDIR "/home/anton/apps/boost_1_71_0/stage/lib")
set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ALL_DYN_LINK OFF)

find_package (Boost REQUIRED COMPONENTS system filesystem)
if (Boost_FOUND)
    include_directories (SYSTEM ${Boost_INCLUDE_DIR})
endif ()


#include(FindPkgConfig)
#pkg_check_modules(LIBMYSQLCLIENT REQUIRED mysqlclient)
#
#foreach(FLAG ${LIBMYSQLCLIENT_CFLAGS_OTHER})
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAG}")
#endforeach()
#
#link_directories(${LIBMYSQLCLIENT_LIBRARY_DIRS})
#link_directories(${mysqlcppconn8_LIBRARY_DIRS})


add_executable(runServer src/main.cpp
        src/listener.cpp
        include/listener.h
        src/fail.cpp
        include/fail.h
        src/http_session.cpp
        include/http_session.h
        src/websocket_session.cpp
        include/websocket_session.h src/runner.cpp
        include/runner.h
        src/view_interface.cpp
        include/view_interface.h tests/gmock/listener.h src/bd_tables_conectors.cpp include/bd_tables_conectors.h include/runner_interface.h include/listener_interfece.h include/http_session_interface.h include/websocket_session_interface.h src/router_interface.cpp include/router_interface.h include/auth_middleware.h include/auth_middleware_interface.h
        src/auth_middleware.cpp)

set(EXTRA_LIBRARY "mysqlcppconn")
find_library(MYSQL_LIB mysqlcppconn)
#target_link_libraries(runServer ${MYSQL_LIB})
#find_library(libmysqlcppconn8 mysqlcppconn8)

target_link_libraries(runServer ${Boost_LIBRARIES} pthread
        ${LIBMYSQLCLIENT_INCLUDE_DIRS}
        ${LIBMYSQLCLIENT_LIBRARIES}
        ${mysqlcppconn8_INCLUDE_DIRS}
        ${mysqlcppconn8_LIBRARIES}
        ${MYSQL_LIB})



#find_package(GTest REQUIRED)
#include_directories(${GTEST_INCLUDE_DIRS})

#find_package(googlegmock REQUIRED)
include_directories(${GMOCK_INCLUDE_DIRS})
set (GMOCK_DIR "/usr/src/googletest")
add_subdirectory(${GMOCK_DIR} ${CMAKE_BINARY_DIR}/googlegmock)
include_directories(SYSTEM ${GMOCK_DIR}/include ${GMOCK_DIR}/gtest/include)
# Link runTests with what we want to test and the GTest and pthread library
#add_executable(runTests tests/gtest_main.cpp
#        tests/gmock/http_session.h
#        listener.cpp
#        listener.h
#        fail.cpp
#        fail.h
#        http_session.cpp
#        http_session.h
#        websocket_session.cpp
#        websocket_session.h
#        runner.cpp
#        runner.h
#        request.cpp
#        request.h
#        tests/test_gmock.cpp
#        tests/gmock/listener.h bd_tables_conectors.cpp bd_tables_conectors.h)
#
#target_link_libraries(runTests
##        ${GTEST_LIBRARIES} pthread
#        ${Boost_LIBRARIES} pthread
#        ${LIBMYSQLCLIENT_INCLUDE_DIRS}
#        ${LIBMYSQLCLIENT_LIBRARIES}
#        gmock_main Sub)

