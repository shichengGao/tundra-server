//
// Created by scgao on 2023/3/4.
//

#ifndef TUNDRA_EVENTLOOP_H
#define TUNDRA_EVENTLOOP_H
#include <atomic>
#include <thread>

namespace tundra {
//one loop per thread
class EventLoop {
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void assertInLoopThread() {
        if (!isInLoopThread()) {
            abortNotInLoopThread();
        }
    }

    bool isInLoopThread() const {
        return threadId_ == std::this_thread::get_id();
    }

    static EventLoop* getEventLoopOfCurrentThread();

private:
    void abortNotInLoopThread();

    std::atomic<bool> looping_;
    const std::thread::id threadId_;

};

}


#endif //TUNDRA_EVENTLOOP_H
