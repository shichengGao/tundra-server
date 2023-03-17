//
// Created by scgao on 2023/3/16.
//

#include "EventLoopThreadPool.h"
#include "net/EventLoop.h"
#include "net/EventLoopThread.h"

namespace tundra{
EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop, const std::string &name)
    : baseLoop_(baseLoop), name_(name), started_(false), numThreads_(0), next_(0) { }

EventLoopThreadPool::~EventLoopThreadPool() {
    //not to delete loop
}

void EventLoopThreadPool::start(const ThreadInitCallback &cb) {
    assert(!started_);
    baseLoop_->assertInLoopThread();

    started_ = true;

    for (int i = 0; i < numThreads_; ++i) {
        char buf[name_.size() + 32];
        snprintf(buf, sizeof(buf), "%s%d", name_.c_str(), i);

        threads_.emplace_back(
                std::make_unique<EventLoopThread>(cb, std::string(buf)));
        loops_.push_back(threads_.back()->startLoop());
    }

    if (numThreads_ == 0 && cb) {
        cb(baseLoop_);
    }
}

EventLoop* EventLoopThreadPool::getNextLoop() {
    baseLoop_->assertInLoopThread();
    assert(started_);

    EventLoop* loop = baseLoop_;
    if (!loops_.empty()) {
        loop = loops_[next_];
        next_ = (next_ + 1) % loops_.size();
    }
    return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops() {
    baseLoop_->assertInLoopThread();
    assert(started_);

    if (loops_.empty()) {
        return std::vector<EventLoop*>(1, baseLoop_);
    } else {
        return loops_;
    }
}





















}
