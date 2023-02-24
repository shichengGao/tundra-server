#cmake_minimum_required(VERSION 3.22)
#project(muduoTrain)
#
#set(CMAKE_CXX_STANDARD 14)
#
#set(LIB_PATH usr/local/lib)
#
#include_directories(/usr/local)

#add_executable(main main.cpp)
add_executable(muduoTrain main.cpp echo.cpp)

target_link_libraries(muduoTrain -lmuduo_net -lmuduo_base )
