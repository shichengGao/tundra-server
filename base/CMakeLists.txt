set(base_SRCS
        AsyncLogger.cpp
        )

add_library(tundra_base ${base_SRCS})
target_link_libraries(tundra_base pthread rt)

add_subdirectory(tests)