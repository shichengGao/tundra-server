//
// Created by scgao on 2023/2/24.
//

#include "TimeStamp.h"
#include <sys/time.h>
#include <inttypes.h>

namespace tundra {

String TimeStamp::toString() const {
    char buf[32] = {0};
    int64_t seconds = microSecondsSinceUnixEpoch_ / kMicroSecondsPerSeconds;
    int64_t microseconds = microSecondsSinceUnixEpoch_ % kMicroSecondsPerSeconds;
    snprintf(buf, sizeof(buf), "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
    return buf;
}

std::string TimeStamp::toFormatString(bool showMicroseconds) const {
    char buf[64] = {0};
    time_t seconds = static_cast<time_t>(microSecondsSinceUnixEpoch_ / kMicroSecondsPerSeconds);
    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);

    if (showMicroseconds)
    {
        int microseconds = static_cast<int>(microSecondsSinceUnixEpoch_ % kMicroSecondsPerSeconds);
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
                 tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                 tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
                 microseconds);
    }
    else
    {
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
                 tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                 tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    }
    return std::string(buf, strlen(buf));
}

TimeStamp TimeStamp::now() {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    int64_t seconds = tv.tv_sec;
    return TimeStamp(seconds * kMicroSecondsPerSeconds + tv.tv_usec);
}

} // tundra