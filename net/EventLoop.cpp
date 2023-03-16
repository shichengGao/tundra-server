//
// Created by scgao on 2023/3/4.
//
#include "base/Logging.h"
#include "EventLoop.h"
#include "net/Channel.h"
#include "net/Poller.h"
#include "net/TimerQueue.h"

#include <poll.h>
#include <sys/eventfd.h>
#include <sstream>
#include <algorithm>

namespace tundra {
thread_local EventLoop* t_loopInThisThread = nullptr;

const int kPollTimeMs = 10000;

int createEventfd() {
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0) {
        Logging::instance().log_error("Failed in eventfd.");
        abort();
    }
    return evtfd;
}


EventLoop::EventLoop()
    : looping_(false), quit_(false),
      eventHandling_(false), callingPendingFunctors_(false),
      iteration_(0), currentActiveChannel_(nullptr),
      poller_(std::make_unique<Poller>(this)),
      timerQueue_(std::make_unique<TimerQueue>(this)),
      threadId_(std::this_thread::get_id()),
      wakeupFd_(createEventfd()),
      wakeupChannel_(std::make_unique<Channel>(this, wakeupFd_))
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

    wakeupChannel_->setReadCallback(
            std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop() {
    std::ostringstream ss;
    ss << "EventLoop " << this << " of thread " << threadId_
       << " destructs in thread " << std::this_thread::get_id();
    Logging::instance().log_debug(ss.str());

    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    close(wakeupFd_);
    t_loopInThisThread = nullptr;
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

    std::ostringstream ss;
    ss << "EventLoop " << this <<" start looping";
    Logging::instance().log_trace(ss.str());

    while(!quit_) {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        ++iteration_;
        //DeBug
//        printActiveChannels();

        eventHandling_ = true;
        for (Channel* channel : activeChannels_) {
            currentActiveChannel_ = channel;
            currentActiveChannel_->handleEvent(pollReturnTime_);
        }
        currentActiveChannel_ = nullptr;
        eventHandling_ = false;
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
    uint64_t one = 1;
    ssize_t n = write(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        std::ostringstream logStream;
        logStream << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
        Logging::instance().log_error(logStream.str());
    }
}

void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = read(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        std::ostringstream logStream;
        logStream << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
        Logging::instance().log_error(logStream.str());
    }
}


void EventLoop::updateChannel(Channel *channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    if (eventHandling_) {
        //channel must be removed by itself?
        assert(currentActiveChannel_ == channel ||
            std::find(activeChannels_.begin(),
                      activeChannels_.end(),
                      channel) == activeChannels_.end());
    }
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    return poller_->hasChannel(channel);
}


TimerId EventLoop::runAt(const tundra::TimeStamp &time, TimerCallback cb) {
    return timerQueue_->addTimer(std::move(cb), time, 0.0);
}

TimerId EventLoop::runAfter(double delay, TimerCallback cb) {
    TimeStamp time(addTime(TimeStamp::now(), delay));
    return runAt(time, std::move(cb));
}

TimerId EventLoop::runEvery(double interval, TimerCallback cb) {
    TimeStamp time(addTime(TimeStamp::now(), interval));
    return timerQueue_->addTimer(std::move(cb), time, interval);
}

void EventLoop::cancel(TimerId timerId) {
    return timerQueue_->cancel(timerId);
}

void EventLoop::runInLoop(Functor cb) {
    if (isInLoopThread()) {
        cb();
    } else {
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(Functor cb) {
    {
        std::lock_guard<std::mutex> lg(mtx_);
        pendingFunctors_.push_back(cb);
    }

    //如果调用者不在当前线程或者此时正在调用functor，都必须唤醒
    if (!isInLoopThread() || callingPendingFunctors_) {
        wakeup();
    }
}

size_t EventLoop::queueSize() const {
    std::lock_guard<std::mutex> lg(mtx_);
    return pendingFunctors_.size();
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

void EventLoop::printActiveChannels() const {
    std::ostringstream logStream;
    for (const Channel* channel : activeChannels_) {
        logStream << "{" << channel->reventsToString() << "} ";
        Logging::instance().log_trace(logStream.str());
        logStream.clear();
    }
}

}
