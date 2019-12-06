cmake_minimum_required(VERSION 3.10)
project(techProject)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")


set(BOOST_INCLUDEDIR "/home/anton/apps/boost_1_71_0")
set(BOOST_LIBRARYDIR "/home/anton/apps/boost_1_71_0/stage/lib")
set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ALL_DYN_LINK OFF)
set(nlohmann_json_DIR "/home/anton/apps/json")
include_directories(include)

set(ALL_SOURCES
        ${SRC_DIR}/AuthMiddleware.cpp
        ${SRC_DIR}/Md5.cpp
        ${SRC_DIR}/Router.cpp
        ${SRC_DIR}/HttpSession.cpp
        ${SRC_DIR}/WebsocketSession.cpp
        ${SRC_DIR}/ViewRegistration.cpp
        ${SRC_DIR}/Fail.cpp
        ${SRC_DIR}/main.cpp
        ${SRC_DIR}/Listener.cpp
        ${SRC_DIR}/View.cpp
        ${SRC_DIR}/Runner.cpp src/Queue.cpp src/ViewOther.cpp)
set(ALL_HEADERS
        ${INC_DIR}/Router.h
        ${INC_DIR}/Md5.h
        ${INC_DIR}/Listener.h
        ${INC_DIR}/ViewRegistration.h
        ${INC_DIR}/Fail.h
        ${INC_DIR}/HttpSession.h
        ${INC_DIR}/WebsocketSession.h
        ${INC_DIR}/Runner.h
        ${INC_DIR}/View.h
        ${INC_DIR}/IRunner.h
        ${INC_DIR}/IListener.h
        ${INC_DIR}/IHttpSession.h
        ${INC_DIR}/IWebsocketSession.h
        ${INC_DIR}/AuthMiddleware.h
        ${INC_DIR}/IAuthMiddleware.h include/Queue.h include/Work.h include/ViewOther.h)


find_package (Boost REQUIRED COMPONENTS system filesystem)
if (Boost_FOUND)
    include_directories (SYSTEM ${Boost_INCLUDE_DIR})
endif ()

find_package (nlohmann_json REQUIRED )
if (nlohmann_json_FOUND)
    include_directories (SYSTEM ${nlohmann_json_INCLUDE_DIR})
else()
    message("error")
endif ()

add_executable(runServer ${ALL_SOURCES} ${ALL_HEADERS})
set(EXTRA_LIBRARY "mysqlcppconn")
find_library(MYSQL_LIB mysqlcppconn)

target_link_libraries(runServer ${Boost_LIBRARIES} pthread
        ${LIBMYSQLCLIENT_INCLUDE_DIRS}
        ${LIBMYSQLCLIENT_LIBRARIES}
        ${mysqlcppconn8_INCLUDE_DIRS}
        ${mysqlcppconn8_LIBRARIES}
        ${MYSQL_LIB}
        ${nlohmann_json}
        nlohmann_json::nlohmann_json)



#find_package(GTest REQUIRED)
#include_directories(${GTEST_INCLUDE_DIRS})

#find_package(googlegmock REQUIRED)
include_directories(${GMOCK_INCLUDE_DIRS})
set (GMOCK_DIR "/usr/src/googletest")
add_subdirectory(${GMOCK_DIR} ${CMAKE_BINARY_DIR}/googlegmock)
include_directories(SYSTEM ${GMOCK_DIR}/include ${GMOCK_DIR}/gtest/include)
# Link runTests with what we want to test and the GTest and pthread library
#add_executable(runTests tests/gtest_main.cpp
#        tests/gmock/HttpSession.h
#        listener.cpp
#        listener.h
#        fail.cpp
#        fail.h
#        HttpSession.cpp
#        HttpSession.h
#        WebsocketSession.cpp
#        WebsocketSession.h
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

