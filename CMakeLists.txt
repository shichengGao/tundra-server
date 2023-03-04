cmake_minimum_required(VERSION 3.22)
project(tundra)

set(CMAKE_CXX_STANDARD 14)

#set(CMAKE_BUILD_TYPE  "Debug")
set(CMAKE_BUILD_TYPE "Release")

# protobuf library
#find_package(Protobuf REQUIRED)
#
#include_directories(${PROTOBUF_INCLUDE_DIR})
#
#target_link_libraries(tundra-server ${PROTOBUF_LIBRARY} )


add_subdirectory(./googletest)
include_directories(./googletest/include)

link_directories(${PROJECT_SOURCE_DIR}/googletest)

include_directories(./)

add_executable(tundra main.cpp)
#add_executable(tundra-server main.cpp echo.cpp)

#target_link_libraries(tundra-server -lmuduo_net -lmuduo_base )


add_subdirectory(tests)

add_subdirectory(util)

add_subdirectory(base)

target_link_libraries(tundra tundra_base)