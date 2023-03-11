//
// Created by scgao on 2023/3/10.
//

#ifndef TUNDRA_ACCEPTOR_H
#define TUNDRA_ACCEPTOR_H

#include <functional>
#include "net/Socket.h"
#include "net/Channel.h"


namespace tundra {

class EventLoop;
class InetAddress;

class Acceptor {
public:
    using NewConnectionCallback =
            std::function<void(int sockfd,const InetAddress&)>;

    Acceptor(EventLoop* loop, const InetAddress& listenAddr);

    //noncopyable
    Acceptor(const Acceptor&) = delete;
    Acceptor operator=(const Acceptor&) = delete;

    void setNewConnectionCallback(const NewConnectionCallback& cb) {
        newConnectionCallback_ = cb;
    }

    bool listenning() const { return listening_; }
    void listen();

private:
    void handleRead();

    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listening_;
    int idleFd_;
};

}


#endif //TUNDRA_ACCEPTOR_H
