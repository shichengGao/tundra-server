//
// Created by scgao on 2023/3/2.
//

#include "LogFile.h"

namespace tundra {

void LogFile::append(const char *logMsgs, int len) {
    assert(len < rollSize_);
    if (cur_bytes_ + len > rollSize_) {
        rollFile();
    }
    curMMap_->append(logMsgs, len);
    cur_bytes_ += len;
}

void LogFile::rollFile() {
    std::string filename = getLogFileName(basename_);

    //update mmap memory
    auto nextMap = std::make_unique<mmapManager>(filename, rollSize_);
    curMMap_.swap(nextMap);
    cur_bytes_ = 0;
}

std::string LogFile::getLogFileName(const std::string &basename) {

    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);

    char time[32];
    std::strftime(time, sizeof(time), "-%Y%m%d-%H%M%S-", &tm);

    char miliseconds[4];
    snprintf(miliseconds, sizeof(miliseconds), "%d", timestamp % 1000);

    return basename + time + miliseconds + ".log";
}

}