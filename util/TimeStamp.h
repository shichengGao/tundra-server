//
// Created by scgao on 2023/2/24.
//

#ifndef TUNDRA_TIMESTAMP_H
#define TUNDRA_TIMESTAMP_H

#include <unistd.h>
#include <stdint.h>
#include "String.h"

namespace tundra {

class TimeStamp {
public:
    //allow implicit conversion
    TimeStamp() : microSecondsSinceUnixEpoch_(0) { }

    explicit TimeStamp(int64_t microSecondsSinceUnixEpoch)
                : microSecondsSinceUnixEpoch_(microSecondsSinceUnixEpoch) { }

    bool valid() const {
        return microSecondsSinceUnixEpoch_ > 0;
    }

    int64_t microSecondsSinceUnixEpoch() const {
        return microSecondsSinceUnixEpoch_;
    }

    time_t secondsSinceUnixEpoch() const {
        return static_cast<time_t>(microSecondsSinceUnixEpoch_ / kMicroSecondsPerSeconds);
    }

    String toString() const;
    std::string toFormatString(bool showMicroseconds = true) const;

    static TimeStamp fromUnixTime(time_t t, int microSeconds) {
        return TimeStamp(static_cast<int64_t>(t) * kMicroSecondsPerSeconds * microSeconds);
    }

    static TimeStamp fromUnixTime(time_t t) {
        return fromUnixTime(t, 0);
    }

    static TimeStamp now();

    static TimeStamp invalid() {
        return TimeStamp();
    }

    static const int64_t kMicroSecondsPerSeconds = 1000 * 1000;

private:
    int64_t microSecondsSinceUnixEpoch_;
};

//define functions out of class while all arguments need to be converted
//refer to chapter 24 of "Effective c++"
bool inline operator<(TimeStamp lhs, TimeStamp rhs) {
    return lhs.microSecondsSinceUnixEpoch() < rhs.microSecondsSinceUnixEpoch();
}

bool inline operator==(TimeStamp lhs, TimeStamp rhs) {
    return lhs.microSecondsSinceUnixEpoch() == rhs.microSecondsSinceUnixEpoch();
}
/* @return
 * return time difference in seconds
 */
double inline timeDiff(TimeStamp early,TimeStamp late) {
    int64_t diff = late.microSecondsSinceUnixEpoch() - early.microSecondsSinceUnixEpoch();
    return static_cast<double>(diff) / TimeStamp::kMicroSecondsPerSeconds;
}

TimeStamp inline addTime(TimeStamp timeStamp, double seconds) {
    int64_t delta = static_cast<int64_t> (seconds * TimeStamp::kMicroSecondsPerSeconds);
    return TimeStamp(timeStamp.microSecondsSinceUnixEpoch() + delta);
}

} // tundra

#endif //TUNDRA_TIMESTAMP_H
