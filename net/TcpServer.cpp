//
// Created by scgao on 2023/3/13.
//

#include "TcpServer.h"
#include "base/Logging.h"
#include "net/EventLoop.h"
#include "net/InetAddress.h"
#include "net/Acceptor.h"
#include "net/TcpConnection.h"

#include <sstream>

namespace tundra {


TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr, const std::string& name)
    : loop_(loop), name_(name), acceptor_(std::make_unique<Acceptor>(loop_, listenAddr)),
    connectionCallback_(), messageCallback_(), nextConnId_(1)
{
    acceptor_->setNewConnectionCallback(
            std::bind(&TcpServer::newConnection, this, _1, _2));
}

void TcpServer::newConnection(int sockfd, const tundra::InetAddress &peerAddr) {
    loop_->assertInLoopThread();
    char buf[32];
    snprintf(buf, sizeof buf, "#%d", nextConnId_);
    ++nextConnId_;

    std::string connName = name_ + buf;

    std::ostringstream ss;
    ss << "TcpServer::newConnection [" << name_ << "] - new connection ["
       << connName <<"] from " << peerAddr.toIpPort();
    Logging::instance().log_info(ss.str());

    InetAddress localAddr(InetAddress::getLocalAddr(sockfd));

    TcpConnectionPtr conn = std::make_shared<TcpConnection>(
            loop_, connName, sockfd, localAddr, peerAddr);

    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setCloseCallback(
            std::bind(&TcpServer::removeConnection, this, _1));
    conn->connectEstablished();
}

void TcpServer::removeConnection(const tundra::TcpConnectionPtr &conn) {
    loop_->assertInLoopThread();

    std::ostringstream ss;
    ss << "TcpServer::remobeConnection [" << name_
       <<"] - connection " << conn->name();
    Logging::instance().log_info(ss.str());

    size_t n = connections_.erase(conn->name());
    assert(n == 1);

    loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

}
