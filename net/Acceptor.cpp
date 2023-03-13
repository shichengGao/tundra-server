//
// Created by scgao on 2023/3/10.
//

#include "Acceptor.h"
#include "net/EventLoop.h"
#include "net/InetAddress.h"

#include <unistd.h>
#include <fcntl.h>
namespace tundra {

Acceptor::Acceptor(tundra::EventLoop *loop, const tundra::InetAddress &listenAddr)
    : loop_(loop), acceptSocket_(Socket::createNonBlockingSocket()),
      acceptChannel_(loop, acceptSocket_.fd()), listening_(false),
      idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    assert(idleFd_ >= 0);
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.bind(listenAddr);
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

void Acceptor::listen() {
    loop_->assertInLoopThread();
    listening_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading();
}

void Acceptor::handleRead() {
    loop_->assertInLoopThread();
    InetAddress peerAddr(0);
    int connfd = acceptSocket_.accept(&peerAddr);

    if (connfd >= 0) {
        if (newConnectionCallback_) {
            newConnectionCallback_(connfd, peerAddr);
        } else {
            close(connfd);
        }
    }
}


}






































