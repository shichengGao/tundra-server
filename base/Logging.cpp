//
// Created by scgao on 2023/3/6.
//
#include "Logging.h"
#include "base/LogFile.h"
namespace tundra {

std::string Logging::basename_ = "default_log";
off_t Logging::rollSize_ = LogFile::defaultRollSize;
int Logging::flushInterval_ = 3;


AsyncLogger& Logging::instance() {
    static AsyncLogger logInstance(basename_, rollSize_, flushInterval_);
    return logInstance;
}

void Logging::InitLoggingConfig(const std::string& basename,
                              off_t rollSize, int flushInterval) {
    basename_ = basename;
    rollSize_ = rollSize;
    flushInterval_ = flushInterval;
    instance(); //initialize
}

}

