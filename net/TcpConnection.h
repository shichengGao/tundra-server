//
// Created by scgao on 2023/3/13.
//

#ifndef TUNDRA_TCPCONNECTION_H
#define TUNDRA_TCPCONNECTION_H

#include <memory>
#include <string>
#include <atomic>

#include "net/InetAddress.h"
#include "util/Buffer.h"
#include "util/types.h"

namespace tundra{

class EventLoop;
class Channel;
class Socket;
class Buffer;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>{
public:
    TcpConnection(EventLoop* loop, const std::string& name, int sockfd,
                  const InetAddress& localAddr, const InetAddress& peerAddr);
    ~TcpConnection();

    TcpConnection(const TcpConnection&) = delete;
    TcpConnection operator=(const TcpConnection&) = delete;

    EventLoop* getLoop() const { return loop_;}
    const std::string& name() const {return name_;}
    const InetAddress& localAddress() const { return localAddr_;}
    const InetAddress& peerAddress() const { return peerAddr_;}
    bool connected() const { return state_ == kConnected;}
    bool disconnedted() const {return state_ == kDisconnected;}

    std::string getTcpInfoString() const;

    void send(const void* msg, int len);
    void send(const std::string& msg);
    void send(Buffer* msg);

    void shutdown();

    void forceClose();
    void forceCLoseWithDelay(double seconds);
    void setTcpNoDelay(bool on);

    //reading or not
    void startRead();
    void stopRead();
    bool isReading() const {
        return reading_;
    }


    void setConnectionCallback(const ConnectionCallback& cb) {
        connectionCallback_ = cb;
    }

    void setMessageCallback(const MessageCallback& cb) {
        messageCallback_ = cb;
    }

    void setWriteCompleteCallback(const WriteCompleteCallback& cb) {
        writeCompleteCallback_ = cb;
    }

    void setHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t highWaterMark) {
        highWaterMarkCallback_ = cb;
        highWaterMark_ = highWaterMark;
    }

    void setCloseCallback(const CloseCallback& cb) {
        closeCallback_ = cb;
    }

    Buffer* inputBuffer() {
        return &inputBuffer_;
    }

    Buffer* outputBuffer() {
        return &outputBuffer_;
    }


    void connectEstablished();

    void connectDestroyed();
private:
    enum StateE{ kConnecting, kConnected, kDisconnecting, kDisconnected };
    void setState(StateE s) { state_ = s; }
    const char* stateToString() const;
    void handleRead(TimeStamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();

    void sendInLoop(const std::string& msg);
    void sendInLoop(const void* msg, int len);
    void shutdownInLoop();

    void forceCLoseInLoop();
    void startReadInLoop();
    void stopReadInLoop();


    EventLoop* loop_;
    std::string name_;
    std::atomic<StateE> state_;
    bool reading_;

    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;

    InetAddress localAddr_;
    InetAddress peerAddr_;


    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    HighWaterMarkCallback highWaterMarkCallback_;
    CloseCallback closeCallback_;
    size_t highWaterMark_;
    Buffer inputBuffer_;
    Buffer outputBuffer_;
};



}

#endif //TUNDRA_TCPCONNECTION_H
