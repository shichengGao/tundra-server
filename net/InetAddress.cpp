//
// Created by scgao on 2023/3/10.
//

#include "InetAddress.h"
#include "base/Logging.h"

#include <netdb.h>
#include <arpa/inet.h>

static const in_addr_t kInaddrAny = INADDR_ANY;
static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;

namespace tundra {
InetAddress::InetAddress(uint16_t port, bool loopbackOnly) {
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    in_addr_t ip = loopbackOnly ? kInaddrLoopback : kInaddrAny;
    addr_.sin_addr.s_addr = htonl(ip);
    addr_.sin_port = htons(port);
}

InetAddress::InetAddress(const char *ip, uint16_t port) {
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    if ((addr_.sin_addr.s_addr = inet_addr(ip)) == INADDR_NONE) {
        Logging::instance().log_error("cannot convert ip form string format to binary.");
    }
}

std::string InetAddress::toIpPort() const {
    std::string ip = toIp();

    char buf[8];
    snprintf(buf, sizeof(buf), ":%u", ntohs(addr_.sin_port));
    std::string port(buf);

    return ip + port;
}

std::string InetAddress::toIp() const {
    return std::string(inet_ntoa(addr_.sin_addr));
}

uint16_t InetAddress::port() const {
    return ntohs(addr_.sin_port);
}

static thread_local char t_resolveBuffer[64 * 1024];

bool InetAddress::resolve(const char *hostname, tundra::InetAddress *result) {
    assert(result);
    struct hostent hent;
    struct hostent* he = nullptr;
    int herrno = 0;
    memset(&hent, 0, sizeof(hent));

    int ret = gethostbyname_r(hostname, &hent, t_resolveBuffer, sizeof(t_resolveBuffer), &he, &herrno);
    if (ret == 0 && he ) {
        assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));   //ipv4
        result->addr_.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
        return true;
    } else {
        if (ret)
            Logging::instance().log_error("InetAddress::resolve error.");
        return false;
    }
}

struct sockaddr_in InetAddress::getLocalAddr(int sockfd) {
    struct sockaddr_in localaddr;
    memset(&localaddr, 0, sizeof(localaddr));
    socklen_t addrLen = sizeof(localaddr);

    if (getsockname(sockfd, reinterpret_cast<struct sockaddr*>(&localaddr), &addrLen) < 0) {
        Logging::instance().log_error("InetAddress::getLocalAddr error.");
    }
    return localaddr;
}

}
