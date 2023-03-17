//
// Created by scgao on 2023/3/8.
//

#ifndef TUNDRA_EVENTLOOPTHREAD_H
#define TUNDRA_EVENTLOOPTHREAD_H

#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace tundra {

class EventLoop;

class EventLoopThread {
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(),
                    const std::string &name = std::string());

    ~EventLoopThread();

    EventLoopThread(const EventLoopThread&) = delete;
    EventLoopThread operator=(const EventLoopThread&) = delete;

    EventLoop *startLoop();

private:
    void threadFunc();

    EventLoop *loop_ __attribute__((guarded_by(mtx_)));
    bool exiting_;
    std::thread thread_;
    std::mutex mtx_;
    std::condition_variable cond_ __attribute__((guarded_by(mtx_)));
    ThreadInitCallback callback_;
};

}


#endif //TUNDRA_EVENTLOOPTHREAD_H
