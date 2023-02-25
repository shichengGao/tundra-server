//
// Created by 郜事成 on 2023/2/14.
//
#include "echo.h"

#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

using namespace  muduo::net;
using muduo::Timestamp;
using muduo::string;

void runEchoServer() {
    LOG_INFO << "pid = "<< getpid();
    muduo::net::EventLoop loop;
    muduo::net::InetAddress listenAddr(2007);
    EchoServer server(&loop, listenAddr);
    server.start();
    loop.loop();
}

void disacrd_onMessage(const TcpConnectionPtr& conn,
                       Buffer* buf,
                       Timestamp time) {
    string msg(buf->retrieveAllAsString());
    LOG_INFO << conn->name() << " discards " << msg.size()
    << " bytes received at " << time.toString();
}

void daytime_onMessage(const TcpConnectionPtr& conn) {
        LOG_INFO << "Daytime Server - " << conn->peerAddress().toIpPort()<<  " -> "
                 << conn->localAddress().toIpPort() << " is "
                 << (conn->connected() ? "UP" : "DOWN");
        if (conn->connected()) {
            conn->send(Timestamp::now().toFormattedString() + "\n");
            conn->shutdown();
        }
}


int main(int argc, char* argv[]) {
//    runEchoServer();
    LOG_INFO << "pid : " << getpid();
    InetAddress listenAddr(9000);
    EventLoop loop;
    TcpServer server(&loop, listenAddr, "complex server");

    server.setConnectionCallback(&daytime_onMessage);
    server.start();
    loop.loop();

}
