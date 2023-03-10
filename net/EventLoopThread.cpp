//
// Created by scgao on 2023/3/8.
//

#include "EventLoopThread.h"
#include "net/EventLoop.h"
#include <assert.h>

namespace tundra{
EventLoopThread::EventLoopThread(const ThreadInitCallback &cb,
                                 const std::string &name)
 : loop_(nullptr), exiting_(false), callback_(cb) {

}



EventLoop* EventLoopThread::startLoop() {
    thread_ = std::thread(&EventLoopThread::threadFunc, this);

    {
        std::unique_lock<std::mutex> ulk(mtx_);
        while (!loop_) {
            cond_.wait(ulk);
        }
    }

    return loop_;
}


void EventLoopThread::threadFunc() {
    EventLoop loop;

    if (callback_) {
        callback_(&loop);
    }

    {
        std::unique_lock<std::mutex> ulk(mtx_);
        loop_ = &loop;
        cond_.notify_all();
    }

    loop.loop();
    std::lock_guard<std::mutex> lg(mtx_);
    loop_ = nullptr;
}



}


























