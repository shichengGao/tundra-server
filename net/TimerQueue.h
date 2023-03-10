//
// Created by scgao on 2023/3/8.
//

#ifndef TUNDRA_TIMERQUEUE_H
#define TUNDRA_TIMERQUEUE_H

#include <set>
#include <vector>
#include <mutex>
#include <atomic>
#include <functional>

#include "util/types.h"
#include "util/TimeStamp.h"
#include "net/Channel.h"


namespace tundra{

class EeventLoop;
class Timer;
class TimerId;

///best efforts timer queue.
///NOo guarantee that the callback will be on time

class TimerQueue {
public:
    explicit TimerQueue(EeventLoop* loop);
    ~TimerQueue();

    TimerId addTimer(const TimerCallback& cb,
                     TimeStamp when, double interval);


    void cancel(TimerId timerId);

private:
    using Entry = std::pair<TimeStamp, Timer*>;
    using TimerList = std::set<Entry>;
    using ActiveTimer = std::pair<Timer*, int64_t>;
    using ActiveTimerSet = std::set<ActiveTimer>;

    void addTimerInLoop(Timer* timer);
    void cancelInLoop(TimerId timerId);

    //called when timerfd alarms
    void handleRead();

    //move out all expired timers
    std::vector<Entry> getExpired(TimeStamp now);
    void reset(const std::vector<Entry>& expired, TimeStamp now);

    bool insert(Timer* timer);

    tundra::EventLoop* loop_;
    const int timerfd_;
    Channel timerfdChannel_;
    //Timer list sorted by expiration
    TimerList timers_;

    //for cancel()
    ActiveTimerSet activeTimers_;
    std::atomic<bool> callingExpiredTimers_;
    ActiveTimerSet cancelingTimers_;


};

}


#endif //TUNDRA_TIMERQUEUE_H
