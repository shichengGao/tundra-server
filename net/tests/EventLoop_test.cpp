//
// Created by scgao on 2023/3/6.
//

#include "net/EventLoop.h"
#include "net/Channel.h"
#include "net/Poller.h"

#include "gtest/gtest.h"
#include "base/Logging.h"
#include <sys/timerfd.h>

using namespace tundra;

TEST(composition_test, smoke_test) {
    EventLoop* g_loop;

    auto timeout = [&]{
        printf("Timeout!\n");
        g_loop->quit();
    };

    EventLoop loop;
    g_loop = &loop;

    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(&loop, timerfd);
    channel.setReadCallback(timeout);
    channel.enableReading();

    struct itimerspec howlong;
    bzero(&howlong, sizeof(howlong));
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd, 0, &howlong, NULL);

    loop.loop();

    ::close(timerfd);

}

int main(int argc, char* argv[]) {
    Logging::InitLoggingConfig("EventLoop_test", Logging::rollSize_, Logging::flushInterval_);
    Logging::instance().setLevel(tundra::AsyncLogger::TRACE);
    Logging::instance().start();


    testing::InitGoogleTest();
    RUN_ALL_TESTS();


    Logging::instance().stop();
    system("rm EventLoop_test*.log");
}