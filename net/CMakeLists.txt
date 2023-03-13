set(net_SRCS
        Timer.cpp
        TimerQueue.cpp
        EventLoop.cpp
        Channel.cpp
        Poller.cpp
        InetAddress.cpp
        Socket.cpp
        Acceptor.cpp
        TcpConnection.cpp
        TcpServer.cpp
        )

add_library(tundra_net STATIC ${net_SRCS})

target_link_libraries(tundra_net tundra_base)

add_subdirectory(tests)