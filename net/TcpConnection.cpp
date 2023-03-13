//
// Created by scgao on 2023/3/13.
//

#include "TcpConnection.h"
#include "net/Channel.h"
#include "net/EventLoop.h"
#include "net/InetAddress.h"
#include "net/Socket.h"
#include "util/Buffer.h"
#include "base/Logging.h"

#include <sstream>

namespace tundra {

TcpConnection::TcpConnection(tundra::EventLoop *loop, const std::string& name, int sockfd,
                             const tundra::InetAddress &localAddr, const tundra::InetAddress &peerAddr)
    : loop_(loop), name_(name), state_(kConnecting),
      socket_(std::make_unique<Socket>(sockfd)),
      channel_(std::make_unique<Channel>(loop, sockfd)),
      localAddr_(localAddr), peerAddr_(peerAddr)
{
    channel_->setReadCallback(
            std::bind(&TcpConnection::handleRead, this));

    std::ostringstream ss;
    ss << "TcpConnection::ctor[" << name_ << "] at " << this << " fd=" << sockfd;
    Logging::instance().log_debug(ss.str());

    socket_->setKeepAlive(true);
}


void TcpConnection::handleRead() {
    Buffer buf;
    ssize_t n = ::read(channel_->fd(), buf.beginWrite(), buf.writableBytes());
    if (n > 0) {
        messageCallback_(shared_from_this(), &buf, TimeStamp::now());
    } else if (n == 0) {
        handleClose();
    } else {
        handleError();
    }
}

void TcpConnection::handleClose() {
    loop_->assertInLoopThread();
    std::ostringstream ss;
    ss << "TcpConnection::handleCLose state = " << state_;
    Logging::instance().log_trace(ss.str());
    assert(state_ == kConnected);
    channel_->disableAll();
    closeCallback_(shared_from_this());
}

void TcpConnection::handleError() {
    int optval;
    socklen_t optLen = sizeof(optval);
    if (getsockopt(channel_->fd(), SOL_SOCKET, SO_ERROR, &optval, &optLen) < 0) {
        optval = errno;
    }

    std::ostringstream ss;
    char buf[512];
    strerror_r(optval, buf, sizeof(buf));
    ss << "TcpConnection::handleError ["<<name_<<"] - SO_ERROR = " << optval
       << " " << buf;

    Logging::instance().log_error(ss.str());
}


void TcpConnection::connectEstablished() {
    loop_->assertInLoopThread();
    assert(state_ == kConnecting);
    setState(kConnected);
    //FIXME: imple this function
//    channel_->tie(shared_from_this())
    channel_->enableReading();

    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed() {
    loop_->assertInLoopThread();
    assert(state_ == kConnected);
    setState(kDisconnected);
    channel_->disableAll();
    connectionCallback_(shared_from_this());

    loop_->removeChannel(channel_.get());
}

}
