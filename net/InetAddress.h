//
// Created by scgao on 2023/3/10.
//

#ifndef TUNDRA_INETADDRESS_H
#define TUNDRA_INETADDRESS_H

#include <netinet/in.h>
#include <string>

namespace tundra {

class InetAddress {
public:
    //for tcp server sockets
    explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false);

    //for tcp client sockets
    explicit InetAddress(const char* ip, uint16_t port);

    explicit InetAddress(const struct sockaddr_in& addr)
            : addr_(addr) { }

    sa_family_t family() const { return addr_.sin_family; }
    uint16_t port() const;
    std::string toIp() const;
    std::string toIpPort() const;

    const struct sockaddr* getSockAddr() const { return reinterpret_cast<const struct sockaddr*>(&addr_);}
    void setSockAddrInet(const struct sockaddr_in& addr) { addr_ = addr; }

    uint32_t ipNetEndian() const { return addr_.sin_addr.s_addr; }
    uint16_t portNetEndian() const { return addr_.sin_port; }

    static bool resolve(const char* hostname, InetAddress* result);

private:
    struct sockaddr_in addr_;

};

}



#endif //TUNDRA_INETADDRESS_H
