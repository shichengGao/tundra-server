//
// Created by scgao on 2023/3/4.
//
#include "base/Logging.h"
#include "EventLoop.h"
#include "net/Channel.h"
#include "net/Poller.h"
#include "net/TimerQueue.h"
#include <poll.h>
#include <sstream>
namespace tundra {
thread_local EventLoop* t_loopInThisThread = 0;

const int kPollTimeMs = 10000;

EventLoop::EventLoop()
    : looping_(false), quit_(false),
    poller_(std::make_unique<Poller>(this)),
    timerQueue_(std::make_unique<TimerQueue>(this)),
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
        doPendingFunctors();
    }
    std::ostringstream stream;
    stream << "EventLoop " << this << " stop looping";
    Logging::instance().log_trace(stream.str());

    looping_ = false;
}

void EventLoop::quit() {
    quit_ = true;
    if (!isInLoopThread()) {
        wakeup();
    }
}

void EventLoop::wakeup() {
    //TODO: finished this function
    uint64_t one = 1;
    ssize_t n = NULL; //
}

void EventLoop::updateChannel(tundra::Channel *channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}


TimerId EventLoop::runAt(const tundra::TimeStamp &time, const tundra::TimerCallback &cb) {
    return timerQueue_->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, const tundra::TimerCallback &cb) {
    TimeStamp time(addTime(TimeStamp::now(), delay));
    return runAt(time, cb);
}

TimerId EventLoop::runEvery(double interval, const tundra::TimerCallback &cb) {
    TimeStamp time(addTime(TimeStamp::now(), interval));
    return timerQueue_->addTimer(cb, time, interval);
}

void EventLoop::runInLoop(const Functor& cb) {
    if (isInLoopThread()) {
        cb();
    } else {
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(const Functor &cb) {
    {
        std::lock_guard<std::mutex> lg(mtx_);
        pendingFunctors_.push_back(cb);
    }

    //如果调用者不在当前线程或者此时正在调用functor，都必须唤醒
    if (!isInLoopThread() || callingPendingFunctors_) {
        wakeup();
    }
}

void EventLoop::abortNotInLoopThread() {
    std::ostringstream stream;
    stream << "EventLoop::abortNotInLoopThread - EventLoop " << this
              << " was created in threadId_ = " << threadId_
              << ", current thread id = " << std::this_thread::get_id;
    Logging::instance().log_fatal(stream.str());
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors_;
    callingPendingFunctors_ = true;
    {
        std::lock_guard<std::mutex> lg(mtx_);
        functors_.swap(pendingFunctors_);
    }

    for (size_t i=0; i < functors_.size(); ++i) {
        functors_[i]();
    }
    callingPendingFunctors_ = false;
}

}
