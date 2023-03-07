set(net_SRCS
        EventLoop.cpp
        Channel.cpp
        )

add_library(tundra_net STATIC ${net_SRCS})

target_link_libraries(tundra_net tundra_base)

add_subdirectory(tests)