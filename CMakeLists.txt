cmake_minimum_required(VERSION 3.10)
project(techProject)


#set(CMAKE_CXX_FLAGS "-O0 -fprofile-arcs -ftest-coverage")
#set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)
#SET(CMAKE_CXX_FLAGS "-g -O0 -Wall -fprofile-arcs -ftest-coverage")
#SET(CMAKE_C_FLAGS "-g -O0 -Wall -W -fprofile-arcs -ftest-coverage")
#SET(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")


#set(BOOST_INCLUDEDIR "/home/anton/apps/boost_1_71_0")
#set(BOOST_LIBRARYDIR "/home/anton/apps/boost_1_71_0/stage/lib")
set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ALL_DYN_LINK OFF)
set(nlohmann_json_DIR "/home/andrey/CLionProjects/json")
#set(nlohmann_json_DIR "/home/anton/apps/json")
include_directories(include)

set(ALL_SOURCES
        ${SRC_DIR}/AuthMiddleware.cpp
        ${SRC_DIR}/Md5.cpp
        ${SRC_DIR}/Router.cpp
        ${SRC_DIR}/HttpSession.cpp
        ${SRC_DIR}/WebsocketSession.cpp
        ${SRC_DIR}/ViewRegistration.cpp
        ${SRC_DIR}/Fail.cpp
        ${SRC_DIR}/Listener.cpp
        ${SRC_DIR}/View.cpp
        ${SRC_DIR}/Runner.cpp
        ${SRC_DIR}/Queue.cpp
        ${SRC_DIR}/ViewOther.cpp
        ${SRC_DIR}/ViewUser.cpp
        ${SRC_DIR}/ViewEvent.cpp
        ${SRC_DIR}/ViewEventFollow.cpp
        ${SRC_DIR}/ViewUserFollow.cpp
        ${SRC_DIR}/ViewUserHobby.cpp
        ${SRC_DIR}/ViewEventHobby.cpp
        ${SRC_DIR}/ViewFindEvent.cpp
        ${SRC_DIR}/CommonChatView.cpp
        ${SRC_DIR}/MessageChatView.cpp
        ${SRC_DIR}/UserChatView.cpp
        ${SRC_DIR}/SharedState.cpp
        ${SRC_DIR}/ViewUserFolList.cpp)


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
        ${INC_DIR}/IAuthMiddleware.h
        ${INC_DIR}/Queue.h
        ${INC_DIR}/Work.h
        ${INC_DIR}/ViewOther.h
        ${INC_DIR}/ViewUser.h
        ${INC_DIR}/ViewEvent.h
        ${INC_DIR}/ViewEventFollow.h
        ${INC_DIR}/ViewUserFollow.h
        ${INC_DIR}/ViewUserHobby.h
        ${INC_DIR}/ViewEventHobby.h
        ${INC_DIR}/ViewFindEvent.h
        ${INC_DIR}/ViewUserFolList.h
        ${INC_DIR}/CommonChatView.h
        ${INC_DIR}/MessageChatView.h
        ${INC_DIR}/UserChatView.h
        ${INC_DIR}/SharedState.h)


set(TEST_FILES
        #        ${TEST_DIR}/tests.cpp
        ${TEST_DIR}/testViewEvent.cpp
        ${TEST_DIR}/testViewReg.cpp
        ${TEST_DIR}/testViewUser.cpp
        ${TEST_DIR}/testViewEventFollow.cpp tests/testViewUserFollow.cpp tests/testViewUserHobby.cpp tests/testViewEventHobby.cpp)

find_package(Boost COMPONENTS system filesystem date_time REQUIRED)
if (Boost_FOUND)
    include_directories(SYSTEM ${Boost_INCLUDE_DIR})
endif ()

find_package(nlohmann_json REQUIRED)
if (nlohmann_json_FOUND)
    include_directories(SYSTEM ${nlohmann_json_INCLUDE_DIR})
else ()
    message("error")
endif ()

add_executable(runServer ${ALL_SOURCES} ${ALL_HEADERS} ${SRC_DIR}/main.cpp)
set(EXTRA_LIBRARY "mysqlcppconn")
find_library(MYSQL_LIB mysqlcppconn)

target_link_libraries(runServer ${Boost_LIBRARIES} pthread
        ${mysqlcppconn8_INCLUDE_DIRS}
        ${mysqlcppconn8_LIBRARIES}
        ${MYSQL_LIB}
        ${nlohmann_json}
        nlohmann_json::nlohmann_json)


set(GTest_DIR "/home/anton/apps/googletest")
find_package(GTest REQUIRED)


include_directories(${GTEST_INCLUDE_DIRS})

include_directories(SYSTEM ${GMOCK_DIR}/include ${GMOCK_DIR}/gtest/include)
# Link runTests with what we want to test and the GTest and pthread library
add_executable(runTests tests/gtest_main.cpp
        ${ALL_SOURCES} ${ALL_HEADERS} ${TEST_FILES})
#
target_link_libraries(runTests
        ${GTEST_LIBRARIES} pthread
        ${Boost_LIBRARIES} pthread
        ${mysqlcppconn8_INCLUDE_DIRS}
        ${mysqlcppconn8_LIBRARIES}
        ${MYSQL_LIB}
        ${LIBMYSQLCLIENT_INCLUDE_DIRS}
        ${LIBMYSQLCLIENT_LIBRARIES}
        ${nlohmann_json}
        nlohmann_json::nlohmann_json)
add_executable(client some_clients/client.cpp)
target_link_libraries(client pthread)
