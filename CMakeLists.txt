cmake_minimum_required(VERSION 3.22)
project(muduoTrain)

set(CMAKE_CXX_STANDARD 14)

# protobuf library
#find_package(Protobuf REQUIRED)
#
#include_directories(${PROTOBUF_INCLUDE_DIR})
#
#target_link_libraries(muduoTrain ${PROTOBUF_LIBRARY} )

add_subdirectory(./googletest)
include_directories(./googletest/include)

link_directories(${PROJECT_SOURCE_DIR}/googletest)

add_executable(muduoTrain main.cpp)
#add_executable(muduoTrain main.cpp echo.cpp)

#target_link_libraries(muduoTrain -lmuduo_net -lmuduo_base )


add_subdirectory(tests)
