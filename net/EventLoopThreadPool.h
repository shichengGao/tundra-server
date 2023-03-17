//
// Created by scgao on 2023/3/16.
//

#ifndef TUNDRA_EVENTLOOPTHREADPOOL_H
#define TUNDRA_EVENTLOOPTHREADPOOL_H

#include "util/types.h"

#include <string>
#include <memory>
#include <vector>

namespace tundra{
class EventLoop;
class EventLoopThread;

class EventLoopThreadPool {
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    EventLoopThreadPool(EventLoop* baseLoop, const std::string& name);
    ~EventLoopThreadPool();

    EventLoopThreadPool(const EventLoopThreadPool&) = delete;
    EventLoopThreadPool operator=(const EventLoopThreadPool&) = delete;

    void setThreadNum(int numThreads) {
        numThreads_ = numThreads;
    }

    void start(const ThreadInitCallback& cb = ThreadInitCallback());

    /// round-robin
    EventLoop* getNextLoop();

    std::vector<EventLoop*> getAllLoops();

    bool started() const {
        return started_;
    }

    const std::string name() const {
        return name_;
    }

private:
    EventLoop* baseLoop_;
    std::string name_;
    bool started_;
    int numThreads_;
    int next_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop*> loops_;
};

}


#endif //TUNDRA_EVENTLOOPTHREADPOOL_H
