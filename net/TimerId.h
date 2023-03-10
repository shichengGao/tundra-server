//
// Created by scgao on 2023/3/8.
//

#ifndef TUNDRA_TIMERID_H
#define TUNDRA_TIMERID_H

#include <stdint.h>

namespace tundra{
class Timer;

//an opaque identifier
class TimerId {
public:
    TimerId() : TimerId(nullptr, 0) { }

    TimerId(Timer* timer, int64_t seq)
        : timer_(timer), sequence_(seq) {

    }

    friend class TimerQueue;

private:
    Timer* timer_;
    int64_t sequence_;
};

}


#endif //TUNDRA_TIMERID_H
