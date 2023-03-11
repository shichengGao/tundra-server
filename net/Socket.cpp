//
// Created by scgao on 2023/3/10.
//

#include "Socket.h"

#include "net/InetAddress.h"
#include "base/Logging.h"

#include <netinet/in.h>
#include <netinet/tcp.h>

#include <sstream>

namespace tundra {

Socket::~Socket() {
    close(sockfd_);
}

bool Socket::getTcpInfo(struct tcp_info * tcpInfo) const {
    socklen_t len = sizeof(*tcpInfo);
    memset(tcpInfo, 0, len);
    return getsockopt(sockfd_, SOL_TCP, TCP_INFO, tcpInfo, &len) == 0;
}

bool Socket::getTcpInfoString(char *buf, int len) const {
    struct tcp_info tcpInfo;
    
    bool ok = getTcpInfo(&tcpInfo);
    if (ok) {
        snprintf(buf, len, "unrecovered=%u "
                           "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
                           "lost=%u retrans=%u rtt=%u rttvar=%u "
                           "sshthresh=%u cwnd=%u total_retrans=%u",
                 tcpInfo.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
                 tcpInfo.tcpi_rto,          // Retransmit timeout in usec
                 tcpInfo.tcpi_ato,          // Predicted tick of soft clock in usec
                 tcpInfo.tcpi_snd_mss,
                 tcpInfo.tcpi_rcv_mss,
                 tcpInfo.tcpi_lost,         // Lost packets
                 tcpInfo.tcpi_retrans,      // Retransmitted packets out
                 tcpInfo.tcpi_rtt,          // Smoothed round trip time in usec
                 tcpInfo.tcpi_rttvar,       // Medium deviation
                 tcpInfo.tcpi_snd_ssthresh,
                 tcpInfo.tcpi_snd_cwnd,
                 tcpInfo.tcpi_total_retrans);  // Total retransmits for entire connection
    }
    return ok;
}

void Socket::bind(const tundra::InetAddress& localAddr) {

    if (::bind(sockfd_, localAddr.getSockAddr(), sizeof(struct sockaddr)) < 0) {
        Logging::instance().log_fatal("Socket bind() fatal.");
    }
}

void Socket::listen() {
    if (::listen(sockfd_, SOMAXCONN) < 0) {
        Logging::instance().log_fatal("Socket listen() fatal.");
    }
}

int Socket::accept(tundra::InetAddress *peerAddr) {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    memset(&addr, 0, len);

    int clifd = accept4(sockfd_, reinterpret_cast<struct sockaddr*>(&addr), &len, SOCK_NONBLOCK | SOCK_CLOEXEC);

    if (clifd < 0)
    {
        int savedErrno = errno;
        std::ostringstream ss;
        ss << "Socket::accept() error, errno = " << savedErrno;
        Logging::instance().log_error(ss.str());

    } else {
        peerAddr->setSockAddrInet(addr);
    }
    return clifd;
}

void Socket::shutdownWrite() {
    if (::shutdown(sockfd_, SHUT_WR) < 0) {
        Logging::instance().log_error("Socket::shutdownWrite() error.");
    }
}

void Socket::setTcpNoDelay(bool on) {
    int optval = on ? 1 : 0;
    setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

void Socket::setReuseAddr(bool on) {
    int optval = on ? 1 : 0;
    int ret = setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (ret < 0 && on) {
        Logging::instance().log_error("SO_REUSEADDR failed.");
    }
}

void Socket::setKeepAlive(bool on) {
    int optval = on ? 1 : 0;
    setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}
}