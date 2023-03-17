//
// Created by scgao on 2023/3/13.
//

#include "TcpServer.h"
#include "base/Logging.h"
#include "net/EventLoop.h"
#include "net/EventLoopThread.h"
#include "net/Acceptor.h"
#include "EventLoopThreadPool.h"

#include <sstream>

namespace tundra {


TcpServer::TcpServer(EventLoop *loop,
                     const InetAddress &listenAddr,
                     const std::string& name,
                     OPtion option)
    : loop_(loop), ipPort_(listenAddr.toIpPort()),
      name_(name), acceptor_(std::make_unique<Acceptor>(loop_, listenAddr, option == kReusePort)),
      threadPool_(std::make_shared<EventLoopThreadPool>(loop_, name_)),
      connectionCallback_(defaultConnectionCallback),
      messageCallback_(defaultMessageCallback),
      nextConnId_(1), started_(ATOMIC_FLAG_INIT)
{
    assert(loop);
    acceptor_->setNewConnectionCallback(
            std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer() {
    loop_->assertInLoopThread();

    std::ostringstream logStream;
    logStream << "TcpServer::~TcpServer [" << name_ << "] destructing";
    Logging::instance().log_trace(logStream.str());

    for (auto& item : connections_) {
        TcpConnectionPtr conn(item.second);
        item.second.reset();
        conn->getLoop()->runInLoop(
                std::bind(&TcpConnection::connectDestroyed, conn));
    }
}

void TcpServer::setThreadNum(int numThreads) {
    assert( numThreads >= 0);
    threadPool_->setThreadNum(numThreads);
}

void TcpServer::start() {
    if (!started_.test_and_set()) {
        threadPool_->start(threadInitCallback_);
        assert(!acceptor_->listenning());
        loop_->runInLoop(
                std::bind(&Acceptor::listen, acceptor_.get()));
    }
}

void TcpServer::newConnection(int sockfd, const tundra::InetAddress &peerAddr) {
    loop_->assertInLoopThread();
    EventLoop* ioLoop = threadPool_->getNextLoop();
    char buf[64];
    snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_++);

    std::string connName = name_ + buf;

    std::ostringstream logStream;
    logStream << "TcpServer::newConnection [" << name_ << "] - new connection ["
       << connName <<"] from " << peerAddr.toIpPort();
    Logging::instance().log_info(logStream.str());

    InetAddress localAddr(InetAddress::getLocalAddr(sockfd));

    TcpConnectionPtr conn = std::make_shared<TcpConnection>(
            loop_,
            connName,
            sockfd,
            localAddr,
            peerAddr);

    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(
            std::bind(&TcpServer::removeConnection, this, _1));
    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const tundra::TcpConnectionPtr &conn) {
    loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const tundra::TcpConnectionPtr &conn) {
    loop_->assertInLoopThread();

    std::ostringstream LogStream;
    LogStream << "TcpServer::remobeConnection [" << name_
       <<"] - connection " << conn->name();
    Logging::instance().log_info(LogStream.str());

    size_t n = connections_.erase(conn->name());
    assert(n == 1);

    EventLoop* ioLoop = conn->getLoop();
    ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

}
