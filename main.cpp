#include <iostream>
#include <set>
#include <memory>

#include <unistd.h>

#include "util/TimeStamp.h"
#include "base/Logging.h"
#include "net/InetAddress.h"
#include "net/EventLoop.h"
#include "net/Acceptor.h"


using namespace tundra;
using namespace std;

void newConnection(int sockfd, const InetAddress& peerAddr) {
    printf("newConnection(): accepted a new connection from %s\n",
           peerAddr.toIpPort().c_str());
    Logging::instance().log_info("new conn!");

    auto time = TimeStamp::now().toFormatString();

    write(sockfd, time.c_str(), time.size());
    close(sockfd);
}

void mainCase(){
    printf("main(): pid = %d\n", getpid());

    InetAddress listenAddr(9981);
    EventLoop loop;


    Acceptor acceptor(&loop, listenAddr);
    acceptor.setNewConnectionCallback(newConnection);
    acceptor.listen();
    loop.loop();
}

int main() {
    Logging::InitLoggingConfig("Acceptor_test", Logging::rollSize_, Logging::flushInterval_);
    Logging::instance().start();
    Logging::instance().log_info("START.");


    mainCase();

    Logging::instance().stop();
    return 0;
}
