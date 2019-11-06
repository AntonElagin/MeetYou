cmake_minimum_required(VERSION 3.10)
project(techProject)

set(CMAKE_CXX_STANDARD 11)

set(BOOST_INCLUDEDIR "/media/anton/files/boost_1_71_0")
set(BOOST_LIBRARYDIR "/media/anton/files/boost_1_71_0/stage/lib")
set (Boost_NO_SYSTEM_PATHS ON)
set (Boost_USE_MULTITHREADED ON)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_STATIC_RUNTIME OFF)
set (BOOST_ALL_DYN_LINK OFF)


find_package (Boost REQUIRED COMPONENTS system filesystem)
if (Boost_FOUND)
    include_directories (SYSTEM ${Boost_INCLUDE_DIR})
endif ()


include(FindPkgConfig)
pkg_check_modules(LIBMYSQLCLIENT REQUIRED mysqlclient)

foreach(FLAG ${LIBMYSQLCLIENT_CFLAGS_OTHER})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAG}")
endforeach()

link_directories(${LIBMYSQLCLIENT_LIBRARY_DIRS})


add_executable(ticker main.cpp)
target_link_libraries(ticker ${Boost_LIBRARIES} pthread
        ${LIBMYSQLCLIENT_INCLUDE_DIRS}
        ${LIBMYSQLCLIENT_LIBRARIES})

#target_include_directories(ticker PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS})
#target_link_libraries(ticker PRIVATE ${LIBMYSQLCLIENT_LIBRARIES})



