//
// Created by scgao on 2023/3/4.
//
#include "base/Logging.h"
#include "EventLoop.h"
#include "net/Channel.h"
#include "net/Poller.h"
#include <poll.h>
#include <sstream>
namespace tundra {
thread_local EventLoop* t_loopInThisThread = 0;

const int kPollTimeMs = 10000;

EventLoop::EventLoop()
    : looping_(false), quit_(false),
    poller_(std::make_unique<Poller>(this)),
    threadId_(std::this_thread::get_id())
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
    quit_ = false;

    while(!quit_) {
        activeChannels_.clear();
        poller_->poll(kPollTimeMs, &activeChannels_);
        for (ChannelList::const_iterator it = activeChannels_.begin();
            it != activeChannels_.end(); ++it) {
            (*it)->handleEvent();
        }
    }
    std::ostringstream stream;
    stream << "EventLoop " << this << " stop looping";
    Logging::instance().log_trace(stream.str());

    looping_ = false;
}

void EventLoop::quit() {
    quit_ = true;
}

void EventLoop::updateChannel(tundra::Channel *channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}

void EventLoop::abortNotInLoopThread() {
    std::ostringstream stream;
    stream << "EventLoop::abortNotInLoopThread - EventLoop " << this
              << " was created in threadId_ = " << threadId_
              << ", current thread id = " << std::this_thread::get_id;
    Logging::instance().log_fatal(stream.str());
}

}
