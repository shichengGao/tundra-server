//
// Created by scgao on 2023/3/11.
//

#include "base/Logging.h"
#include "net/Acceptor.h"
#include "net/InetAddress.h"
#include "net/EventLoop.h"
#include "gtest/gtest.h"

using namespace std;
using namespace tundra;


void newConnection(int sockfd, const InetAddress& peerAddr) {
    printf("newConnection(): accepted a new connection from %s\n",
           peerAddr.toIpPort().c_str());
    write(sockfd, "How are you?\n", 13);
    close(sockfd);
}

/// cannot stop normally, caused logging error.
TEST(acceptor_function_test, simple_case) {
    printf("main(): pid = %d\n", getpid());

    InetAddress listenAddr(9981);
    EventLoop loop;

    Acceptor acceptor(&loop, listenAddr);
    acceptor.setNewConnectionCallback(newConnection);
    acceptor.listen();
    loop.loop();

    loop.quit();
}

int main(int argc, char* argv[]) {
    Logging::InitLoggingConfig("Acceptor_test", Logging::rollSize_, Logging::flushInterval_);
    Logging::instance().start();
    Logging::instance().log_info("START.");

    testing::InitGoogleTest();

    int ret = RUN_ALL_TESTS();

    Logging::instance().stop();
    return ret;
}


















