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
#include "base/WeakCallback.h"

#include <sstream>

namespace tundra {

void defaultConnectionCallback(const TcpConnectionPtr& conn)
{
    std::ostringstream logStream;
    logStream << conn->localAddress().toIpPort() << " -> "
              << conn->peerAddress().toIpPort() << " is "
              << (conn->connected() ? "UP" : "DOWN");
}

void defaultMessageCallback(const TcpConnectionPtr&,
                            Buffer* buf, TimeStamp)
{
    buf->retrieveAll();
}

TcpConnection::TcpConnection(tundra::EventLoop *loop, const std::string& name, int sockfd,
                             const tundra::InetAddress &localAddr, const tundra::InetAddress &peerAddr)
    : loop_(loop), name_(name), state_(kConnecting),reading_(true),
      socket_(std::make_unique<Socket>(sockfd)),
      channel_(std::make_unique<Channel>(loop, sockfd)),
      localAddr_(localAddr), peerAddr_(peerAddr),
      highWaterMark_(64 * 1024 * 1024)
{
    channel_->setReadCallback(
            std::bind(&TcpConnection::handleRead, this, _1));
    channel_->setWriteCallback(
            std::bind(&TcpConnection::handleWrite, this));
    channel_->setCloseCallback(
            std::bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(
            std::bind(&TcpConnection::handleError, this));

    std::ostringstream logStream;
    logStream << "TcpConnection::ctor[" << name_ << "] at " << this << " fd=" << sockfd;
    Logging::instance().log_debug(logStream.str());

    socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection() {
    std::ostringstream logStream;
    logStream << "TcpConnection::dtor[" << name_ << "] at "
              << this << " fd=" << channel_->fd()
              << " state=" << stateToString();
    assert(state_ == kDisconnected);
}

bool TcpConnection::getTcpInfo(struct tcp_info *tcpInfo) const {
    return socket_->getTcpInfo(tcpInfo);
}

std::string TcpConnection::getTcpInfoString() const {
    char buf[1024];
    buf[0] = '\0';
    socket_->getTcpInfoString(buf, sizeof(buf));
    return buf;
}

void TcpConnection::shutdown() {
    //Compare and swap
    StateE pred = kConnected;
    bool ok = std::atomic_compare_exchange_strong(&state_, &pred, kDisconnecting);
    if (!ok) {
        Logging::instance().log_error("TcpConnection::shutdown CAS error.");
    } else {
        loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::shutdownInLoop() {
    loop_->assertInLoopThread();
    if (!channel_->isWriting()) {
        socket_->shutdownWrite();
    }

}

void TcpConnection::send(tundra::Buffer *buf) {
    if (state_ == kConnected) {
        if (loop_->isInLoopThread()) {
            sendInLoop(buf->peek(), buf->readableBytes());
            buf->retrieveAll();
        } else {
            loop_->runInLoop(
                    std::bind(static_cast<void(TcpConnection::*)(const std::string&)>
                              (&TcpConnection::sendInLoop), this, buf->retrieveAllAsString().string()));
        }
    }
}

void TcpConnection::send(const std::string& msg) {
    send(msg.c_str(), msg.size());
}

void TcpConnection::send(const void *msg, int len) {
    if (state_ == kConnected) {
        if (loop_->isInLoopThread()) {
            sendInLoop(msg, len);
        } else {
            loop_->runInLoop(
                    std::bind(static_cast<void (TcpConnection::*)(const void *, int)>
                              (&TcpConnection::sendInLoop), this, msg, len));
        }
    }
}

void TcpConnection::sendInLoop(const std::string &msg) {
    sendInLoop(msg.c_str(), msg.size());
}

void TcpConnection::sendInLoop(const void *msg, int len) {
    loop_->assertInLoopThread();
    ssize_t nwrote = len;
    ssize_t remaining = len;
    bool faultError = false;
    if (state_ == kDisconnected) {
        Logging::instance().log_warn("disconnected, give up writting.");
        return;
    }

    if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
        nwrote = write(channel_->fd(), msg, len);
        if (nwrote >= 0) {
            remaining = len - nwrote;
            if (remaining == 0 && writeCompleteCallback_) {
                loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
            }
        } else {
            nwrote = 0;
            if (errno != EWOULDBLOCK)
            {
                Logging::instance().log_error("TcpConnection::sendInLoop error.");
                if (errno == EPIPE || errno == ECONNRESET)
                {
                    faultError = true;
                }
            }
        }
    }

    assert(remaining <= len);
    if (!faultError && remaining > 0) {
        size_t oldLen = outputBuffer_.readableBytes();
        if (oldLen + remaining >= highWaterMark_
            && oldLen < highWaterMark_
            && highWaterMarkCallback_)
        {
            loop_->queueInLoop(std::bind(highWaterMarkCallback_,
                                         shared_from_this(),
                                         oldLen + remaining));
        }
        outputBuffer_.append(reinterpret_cast<const char*>(msg) + nwrote, remaining);
        if (!channel_->isWriting()) {
            channel_->enableWriting();
        }
    }
}

void TcpConnection::forceClose() {
    if (state_ == kConnected || state_ == kDisconnecting) {
        setState(kDisconnecting);
        loop_->queueInLoop(
                std::bind(&TcpConnection::forceCLoseInLoop, shared_from_this()));
    }
}

void TcpConnection::forceCLoseWithDelay(double seconds) {
    if (state_ == kConnected || state_ == kDisconnecting) {
        setState(kDisconnecting);
        loop_->runAfter(
                seconds,
                makeWeakCallback(shared_from_this(),
                                 &TcpConnection::forceClose));
    }
}

void TcpConnection::forceCLoseInLoop() {
    loop_->assertInLoopThread();
    if (state_ == kConnected || state_ == kDisconnecting) {
        handleClose();
    }
}

const char* TcpConnection::stateToString() const {
    switch ( state_ ) {
        case  kDisconnected:
            return "kDisconnected";
        case kConnected:
            return "kConnected";
        case kConnecting:
            return "Connecting";
        case kDisconnecting:
            return "kDisconnecting";
        default:
            return "unknown state";
    }
}

void TcpConnection::setTcpNoDelay(bool on) {
    socket_->setTcpNoDelay(on);
}

void TcpConnection::startRead() {
    loop_->runInLoop(std::bind(&TcpConnection::startReadInLoop, this));
}

void TcpConnection::startReadInLoop() {
    loop_->assertInLoopThread();
    if (!reading_ || !channel_->isReading()) {
        channel_->enableReading();
        reading_ = true;
    }
}

void TcpConnection::stopRead() {
    loop_->runInLoop(std::bind(&TcpConnection::stopReadInLoop, this));
}

void TcpConnection::stopReadInLoop() {
    loop_->assertInLoopThread();
    if (reading_ || channel_->isReading()) {
        channel_->disableReading();
        reading_ = false;
    }
}


void TcpConnection::handleRead(TimeStamp receiveTime) {
    loop_->assertInLoopThread();
    int savedErrno = 0;
    ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
    if (n > 0) {
        messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
    } else if (n == 0) {
        handleClose();
    } else {
        errno = savedErrno;
        Logging::instance().log_error("TcpConnection::handleRead error.");
        handleError();
    }
}

void TcpConnection::handleWrite() {
    loop_->assertInLoopThread();
    if (channel_->isWriting()) {
        ssize_t  n = write(channel_->fd(),
                           outputBuffer_.peek(),
                           outputBuffer_.readableBytes());
        if (n > 0){
            outputBuffer_.retrieve(n);
            if (outputBuffer_.readableBytes() == 0) {
                channel_->disableWriting();
                if (writeCompleteCallback_) {
                    loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
                }
                if (state_ == kDisconnecting) {
                    shutdownInLoop();
                }
            }
        } else {
            Logging::instance().log_error("TcpConnection::handleWrite");
        }
    } else {
        std::ostringstream logStream;
        logStream << "Connection fd = " << channel_->fd()
                    << " is down, no more writing";
        Logging::instance().log_trace(logStream.str());
    }
}

void TcpConnection::handleClose() {
    loop_->assertInLoopThread();
    std::ostringstream logStream;
    logStream << "fd = " << channel_->fd() << " state = " << stateToString();
    Logging::instance().log_trace(logStream.str());
    assert(state_ == kConnected || state_ == kDisconnecting);

    setState(kDisconnected);
    channel_->disableAll();

    TcpConnectionPtr guardThis(shared_from_this());
    connectionCallback_(guardThis);

    closeCallback_(guardThis);
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
    channel_->tie(shared_from_this());
    channel_->enableReading();

    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed() {
    loop_->assertInLoopThread();
    if (state_ == kConnected) {
        setState(kDisconnected);
        channel_->disableAll();

        connectionCallback_(shared_from_this());
    }
    channel_->remove();
}

}
