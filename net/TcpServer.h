//
// Created by scgao on 2023/3/13.
//

#ifndef TUNDRA_TCPSERVER_H
#define TUNDRA_TCPSERVER_H

#include "util/types.h"
#include <map>

namespace tundra {

class EventLoop;
class Acceptor;
class InetAddress;

class TcpServer {
public:
    TcpServer(EventLoop* loop, const InetAddress& listenAddr, const std::string& name);
    ~TcpServer();

    TcpServer(const TcpServer&) = delete;
    TcpServer operator=(const TcpServer&) = delete;

    /// thread safe
    void start();

    ///set connection callback.
    ///not thrad safe.
    void setConnectionCallback(const ConnectionCallback& cb) {
        connectionCallback_ = cb;
    }

    ///set message callback
    void setMessageCallback(const MessageCallback& cb) {
        messageCallback_ = cb;
    }

private:
    void newConnection(int sockfd, const InetAddress& peerAddr);
    ///thread safe.
    void removeConnection(const TcpConnectionPtr& conn);

    using ConnectionMap = std::map<std::string, TcpConnectionPtr>;

    EventLoop* loop_;
    const std::string name_;
    std::unique_ptr<Acceptor> acceptor_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;

    bool started_;
    int nextConnId_; // always in loop thread
    ConnectionMap connections_; //Tcp connections' name:conn

};


}

#endif //TUNDRA_TCPSERVER_H
