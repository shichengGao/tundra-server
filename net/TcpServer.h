//
// Created by scgao on 2023/3/13.
//

#ifndef TUNDRA_TCPSERVER_H
#define TUNDRA_TCPSERVER_H

#include "net/TcpConnection.h"
#include "util/types.h"
#include <map>

namespace tundra {

class EventLoop;
class EventLoopThreadPool;
class Acceptor;

class TcpServer {
public:

    using ThreadInitCallback = std::function<void(EventLoop*)>;
    enum OPtion{
        kNoReusePort,
        kReusePort
    };

    TcpServer(EventLoop* loop,
              const InetAddress& listenAddr,
              const std::string& name,
              OPtion oPtion = kNoReusePort);
    ~TcpServer();

    TcpServer(const TcpServer&) = delete;
    TcpServer operator=(const TcpServer&) = delete;

    const std::string& ipPort() const { return ipPort_;}
    const std::string& name() const { return name_;}
    EventLoop* getLoop() const { return loop_;}

    void setThreadNum(int numThreads);
    void setThreadInitCallback(const ThreadInitCallback& cb) {
        threadInitCallback_ = cb;
    }

    std::shared_ptr<EventLoopThreadPool> threadPool() {
        return threadPool_;
    }


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

    ///set writeCompleteCallback
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) {
        writeCompleteCallback_ = cb;
    }

private:
    void newConnection(int sockfd, const InetAddress& peerAddr);
    ///thread safe.
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);

    using ConnectionMap = std::map<std::string, TcpConnectionPtr>;

    EventLoop* loop_; // acceptor loop
    const std::string ipPort_;
    const std::string name_;
    std::unique_ptr<Acceptor> acceptor_;
    std::shared_ptr<EventLoopThreadPool> threadPool_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    ThreadInitCallback threadInitCallback_;

    std::atomic_flag started_;

    int nextConnId_; // always in loop thread
    ConnectionMap connections_; //Tcp connections' name:conn

};


}

#endif //TUNDRA_TCPSERVER_H
