//
// Created by scgao on 2023/3/10.
//

#ifndef TUNDRA_SOCKET_H
#define TUNDRA_SOCKET_H

struct tcp_info;

namespace tundra{
class InetAddress;

//the RAII handle of socket file descriptor
class Socket {
public:

    static int createNonBlockingSocket();

    explicit Socket(int sockfd)
        : sockfd_(sockfd) { }

    Socket(const Socket&) = delete;
    Socket operator=(const Socket&) = delete;

    ~Socket();

    int fd() const {
        return sockfd_;
    }

    bool getTcpInfo(struct tcp_info*) const;
    bool getTcpInfoString(char* buf, int len) const;

    void bind(const InetAddress& localAddr);
    void listen();

    int accept(InetAddress* peerAddr);
    void shutdownWrite();

    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

private:
    const int sockfd_;

};


}



#endif //TUNDRA_SOCKET_H
