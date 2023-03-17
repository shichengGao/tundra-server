//
// Created by scgao on 2023/3/8.
//

#include "TimerQueue.h"
#include "net/Timer.h"
#include "net/TimerId.h"
#include "net/Channel.h"
#include "net/EventLoop.h"
#include "base/Logging.h"

#include <unistd.h>
#include <sys/timerfd.h>
#include <functional>

#include <sstream>

namespace tundra {

namespace detail {
int createTimerfd() {
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

    if (timerfd < 0) {
        Logging::instance().log_fatal("Failed in timerfd_create.");
    }

    return timerfd;
}

struct timespec howMuchTimeFromNow(TimeStamp when) {
    int64_t microseconds = when.microSecondsSinceUnixEpoch()
            - TimeStamp::now().microSecondsSinceUnixEpoch();

    if (microseconds < 100) {
        microseconds = 100;
    }

    struct timespec ts;
    ts.tv_sec = static_cast<time_t >(
            microseconds / TimeStamp::kMicroSecondsPerSeconds);
    ts.tv_nsec = static_cast<long>(
            (microseconds % TimeStamp::kMicroSecondsPerSeconds) * 1000);
    return ts;
}

//when wakeup() was called by add 8 bytes to fd, then read it
void readTimerfd(int timerfd, TimeStamp now) {
    uint64_t howmany;
    ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));

    std::ostringstream ss;
    ss << "TimerQueue::handleRead() " << howmany << " at " << now.toString().string();
    Logging::instance().log_trace(ss.str());
    ss.clear();

    if (n != sizeof(howmany)) {
        ss << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
        Logging::instance().log_error(ss.str());
    }
}

void resetTimerfd(int timerfd, TimeStamp expiration) {
    //wake up loop by timerfd_settime()
    struct itimerspec newValue;
    struct itimerspec oldValue;
    memset(&newValue, 0, sizeof( newValue ));
    memset(&oldValue, 0, sizeof( oldValue ));
    newValue.it_value = howMuchTimeFromNow(expiration);
    int ret = timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if (ret) {
        Logging::instance().log_error(" timerfd_settime()");
    }
}
}

TimerQueue::TimerQueue(EventLoop* loop)
    : loop_(loop), timerfd_(detail::createTimerfd()),
      timerfdChannel_(loop, timerfd_), timers_(),
      callingExpiredTimers_(false)
{
    timerfdChannel_.setReadCallback(std::bind(&TimerQueue::handleRead, this));
    timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue() {
    timerfdChannel_.disableAll();
//    timerfdChannel_.remove();
    close(timerfd_);

    for (const Entry& timer : timers_) {
        delete timer.second;
    }
}


TimerId TimerQueue::addTimer(const tundra::TimerCallback &cb, tundra::TimeStamp when, double interval) {
    Timer* timer = new Timer(cb, when, interval);
    loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return TimerId(timer, timer->sequence());
}

void TimerQueue::cancel(TimerId timerId) {
    loop_->runInLoop(std::bind(&TimerQueue::cancelInLoop, this, timerId));
}

void TimerQueue::addTimerInLoop(tundra::Timer *timer) {
    loop_->assertInLoopThread();
    bool earliestChanged = insert(timer);

    if (earliestChanged) {
        detail::resetTimerfd(timerfd_, timer->expiration());
    }
}

void TimerQueue::cancelInLoop(TimerId timerId) {
    loop_->assertInLoopThread();
    assert(timers_.size() == activeTimers_.size());
    ActiveTimer timer(timerId.timer_, timerId.sequence_);

    ActiveTimerSet::iterator it = activeTimers_.find(timer);

    if (it != activeTimers_.end()) {
        size_t n = timers_.erase(Entry(it->first->expiration(), it->first));
        assert(n == 1);
        delete it->first;
        activeTimers_.erase(it);
    } else if (callingExpiredTimers_) {
        cancelingTimers_.insert(timer);
    }

    assert(timers_.size() == activeTimers_.size());

}

void TimerQueue::handleRead() {
    loop_->assertInLoopThread();
    TimeStamp now(TimeStamp::now());
    detail::readTimerfd(timerfd_, now);

    std::vector<Entry> expired = getExpired(now);

    callingExpiredTimers_ = true;
    cancelingTimers_.clear();

    for (const Entry& entry : expired) {
        entry.second->run();
    }
    callingExpiredTimers_ = false;

    reset(expired, now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(TimeStamp now) {
    assert(timers_.size() == activeTimers_.size());
    std::vector<Entry> expired;
    Entry sentry = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerList::iterator it = timers_.lower_bound(sentry);
    assert(it == timers_.end() || now < it->first);
    std::copy(timers_.begin(), it, std::back_inserter(expired));
    timers_.erase(timers_.begin(), it);

    for (const Entry& it : expired) {
        ActiveTimer timer(it.second, it.second->sequence());
        size_t n = activeTimers_.erase(timer);
        assert( n == 1);
    }

    assert(timers_.size() == activeTimers_.size());
    return expired;
}

void TimerQueue::reset(const std::vector<Entry> &expired, tundra::TimeStamp now) {
    TimeStamp nextExpire;

    for(const Entry& it : expired) {
        ActiveTimer timer(it.second, it.second->sequence());
        if (it.second->repeat() &&
            cancelingTimers_.find(timer) == cancelingTimers_.end()) {
            it.second->restart(now);
            insert(it.second);
        } else {
            delete it.second;
        }
    }

    if (!timers_.empty()) {
        nextExpire = timers_.begin()->second->expiration();
    }

    if (nextExpire.valid()) {
        detail::resetTimerfd(timerfd_, nextExpire);
    }
}

bool TimerQueue::insert(tundra::Timer *timer) {
    loop_->assertInLoopThread();
    assert(timers_.size() == activeTimers_.size());
    bool earliestChanged = false;
    TimeStamp when = timer->expiration();
    TimerList::iterator it = timers_.begin();
    if (it == timers_.end() || when < it->first) {
        earliestChanged = true;
    }

    {
        auto result = timers_.insert(Entry(when, timer));
        assert(result.second);
    }

    {
        auto result = activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
        assert(result.second);
    }

    assert(timers_.size() == activeTimers_.size());

    return earliestChanged;
}


}
