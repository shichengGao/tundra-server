//
// Created by scgao on 2023/3/7.
//

#ifndef TUNDRA_TIMER_H
#define TUNDRA_TIMER_H

#include <functional>
#include <atomic>
#include "util/TimeStamp.h"
#include "util/types.h"

namespace tundra{

class Timer {
public:
    Timer(TimerCallback cb, TimeStamp when, double interval)
        : callback_(std::move(cb)),
          expiration_(when), interval_(interval),
          repeat_(interval > 0.0), sequence_(s_numCreated_.fetch_add(1)+1) { }

    void run() const {
        callback_();
    }

    TimeStamp expiration() const { return expiration_; }
    bool repeat() const { return repeat_; }
    int64_t sequence() const { return sequence_; }

    void restart(TimeStamp now);

    static int64_t numCreated() { return s_numCreated_;}

private:
    const TimerCallback callback_;
    TimeStamp expiration_;
    const double interval_;
    const bool repeat_;
    const int64_t sequence_;

    static std::atomic<int64_t> s_numCreated_;
};

}



#endif //TUNDRA_TIMER_H
