set(base_SRCS
        LogFile.cpp
        AsyncLogger.cpp
        )

add_library(tundra_base STATIC ${base_SRCS})

target_link_libraries(tundra_base tundra_util)

add_subdirectory(tests)