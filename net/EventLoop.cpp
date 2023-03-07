//
// Created by scgao on 2023/3/4.
//
#include "base/Logging.h"
#include "EventLoop.h"
#include <poll.h>
#include <sstream>
namespace tundra {
thread_local EventLoop* t_loopInThisThread = 0;

EventLoop::EventLoop()
    : looping_(false), threadId_(std::this_thread::get_id())
{

    std::ostringstream stream;
    stream << "EventLoop created " << this << " in thread " << threadId_;
    Logging::instance().log_trace(stream.str());
    stream.clear();


    if (t_loopInThisThread) {
        stream<<" Another EventLoop " << t_loopInThisThread
            <<" exists in this thread " << threadId_;
        Logging::instance().log_fatal(stream.str());
        stream.clear();
    } else {
        t_loopInThisThread = this;
    }
}

EventLoop::~EventLoop() {
    assert(!looping_);
    t_loopInThisThread = NULL;
}

EventLoop* EventLoop::getEventLoopOfCurrentThread() {
    return t_loopInThisThread;
}

//must run in IO thread
void EventLoop::loop() {
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;

    ::poll(NULL, 0, 5*1000);

    std::ostringstream stream;
    stream << "EventLoop " << this << " stop looping";
    Logging::instance().log_trace(stream.str());

    looping_ = false;
}

void EventLoop::abortNotInLoopThread()
{
    std::ostringstream stream;
    stream << "EventLoop::abortNotInLoopThread - EventLoop " << this
              << " was created in threadId_ = " << threadId_
              << ", current thread id = " << std::this_thread::get_id;
    Logging::instance().log_fatal(stream.str());
}

}
