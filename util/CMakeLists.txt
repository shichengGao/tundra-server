set(util_SRCS
    Buffer.cpp
    Logging.cpp
    mmapManager.cpp
    CountDownLatch.cpp
    TimeStamp.cpp
    )

add_library(tundra_util ${util_SRCS})
target_link_libraries(tundra_util pthread rt)

add_subdirectory(tests)