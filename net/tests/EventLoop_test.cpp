//
// Created by scgao on 2023/3/6.
//

#include "net/EventLoop.h"
#include "gtest/gtest.h"
#include "base/Logging.h"

using namespace tundra;

TEST(EvenLoop_run, simple_two_loop) {
    auto threadFunc = [](){
        printf("thraedFunc(): pid = %d, tid = %lld\n",
               getpid(), std::this_thread::get_id());
        tundra::EventLoop loop;
        loop.loop();
    };

    printf("main() : pid = %d, tid = %lld\n",
           getpid(), std::this_thread::get_id());

    EventLoop loop;

    std::thread subThread(threadFunc);

    loop.loop();

    subThread.join();

    Logging::instance().log_debug("END.");
}

TEST(EvenLoop_run, negative_run) {
    EventLoop* g_loop;
    auto threadFunc = [&](){
        printf("func thread ID : %lld\n", std::this_thread::get_id());
        g_loop->loop();
    };

    EventLoop loop;
    g_loop = &loop;
    printf("now thread ID : %lld\n", std::this_thread::get_id());
    std::thread t(threadFunc);

    t.join();
}

int main(int argc, char* argv[]) {
    Logging::InitLoggingConfig("EventLoop_test", Logging::rollSize_, Logging::flushInterval_);
    Logging::instance().setLevel(tundra::AsyncLogger::TRACE);
    Logging::instance().start();


    testing::InitGoogleTest();
    RUN_ALL_TESTS();


    Logging::instance().stop();
//    system("rm EventLoop_test*.log");
}