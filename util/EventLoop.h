//
// Created by scgao on 2023/2/23.
//

#ifndef TUNDRA_EVENTLOOP_H
#define TUNDRA_EVENTLOOP_H

#include <unistd.h>

namespace tundra {

class EventLoop {
public:
    EventLoop();
    ~EventLoop();

    EventLoop(const EventLoop&) = delete;
    void operator=(const EventLoop&) = delete;

    void loop();

    void assertInLoopThread() {
        if (!isInLoopThread()) {
            abortNotInLoopThread();
        }
    }

    bool isInLoopThread() const { return threadId_ == CurrentThread::tid()};

private:
    void abortNotInLoopThread();
    bool looping_;   //atomic
    const pid_t threadId_;
};

} // tundra

#endif //TUNDRA_EVENTLOOP_H
