//
// Created by 郜事成 on 2023/2/14.
//

#ifndef MUDUOTRAIN_ECHO_H
#define MUDUOTRAIN_ECHO_H


#include <muduo/net/TcpServer.h>

class EchoServer {
public:
    EchoServer(muduo::net::EventLoop* loop, const muduo::net::InetAddress& listenAddr);

    void start();

private:
    void onConnection(const muduo::net::TcpConnectionPtr& conn);

    void onMessage(const muduo::net::TcpConnectionPtr& conn,
                   muduo::net::Buffer* buf,
                   muduo::Timestamp time);

    muduo::net::TcpServer server_;
};


#endif //MUDUOTRAIN_ECHO_H
