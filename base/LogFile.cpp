//
// Created by scgao on 2023/3/2.
//

#include "LogFile.h"
#include <cstring>

namespace tundra {

void LogFile::append(const char *logMsgs, int len) {
    assert(10*len < rollSize_);
    if (cur_bytes_ + len > rollSize_) {
        rollFile();
    }
    memcpy(curMMap_->get()+cur_bytes_, logMsgs, len);
    cur_bytes_ += len;

}



void LogFile::rollFile() {
    time_t now = 0;
    std::string filename = getLogFileName(basename_, &now);
    //update mmap memory
    curMMap_ = std::make_unique<mmapManager>(filename, rollSize_);
    cur_bytes_ = 0;
}

std::string LogFile::getLogFileName(const std::string &basename, time_t *now) {
    std::string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;

    char timebuf[32];
    struct tm tm;
    *now = time(NULL);
    gmtime_r(now, &tm); // FIXME: localtime_r ?
    strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S", &tm);
    filename += timebuf;
    filename += ".log";

    return filename;
}

}