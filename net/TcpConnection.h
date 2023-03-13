//
// Created by scgao on 2023/3/13.
//

#ifndef TUNDRA_TCPCONNECTION_H
#define TUNDRA_TCPCONNECTION_H

#include <memory>
#include <string>
#include <atomic>

#include "net/InetAddress.h"
#include "util/types.h"

namespace tundra{

class EventLoop;
class Channel;
class Socket;

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
//    bool disconnedted() const {return state_ == kDisConnected;}

    void setConnectionCallback(const ConnectionCallback& cb) {
        connectionCallback_ = cb;
    }

    void setMessageCallback(const MessageCallback& cb) {
        messageCallback_ = cb;
    }

    void setCloseCallback(const CloseCallback& cb) {
        closeCallback_ = cb;
    }

    void connectEstablished();

    void connectDestroyed();
private:
    enum StateE{ kConnecting, kConnected, kDisconnected, };
    void setState(StateE s) { state_ = s; }
    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

    EventLoop* loop_;
    std::string name_;
    std::atomic<StateE> state_;

    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;

    InetAddress localAddr_;
    InetAddress peerAddr_;


    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    CloseCallback closeCallback_;
};


}

#endif //TUNDRA_TCPCONNECTION_H
