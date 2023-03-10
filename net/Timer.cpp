//
// Created by scgao on 2023/3/7.
//

#include "Timer.h"

namespace tundra{
    std::atomic<int64_t> s_numCreated_;

void Timer::restart(tundra::TimeStamp now) {
    if (repeat_) {
        expiration_ = addTime(now, interval_);
    } else {
        expiration_ = TimeStamp::invalid();
    }
}

}
