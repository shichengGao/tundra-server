set(base_SRCS
        AsyncLogger.cpp
        LogFile.cpp
        )

add_library(tundra_base ${base_SRCS})
target_link_libraries(tundra_base tundra_util)

add_subdirectory(tests)