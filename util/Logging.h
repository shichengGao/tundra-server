//
// Created by scgao on 2023/2/23.
//
#ifndef TUNDRA_LOGGING_H
#define TUNDRA_LOGGING_H

#include <fstream>
#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <assert.h>

#include "String.h"
#include "CountDownLatch.h"

namespace tundra{

class Logger {
public:
    enum Level {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        LEVEL_COUNT
    };

    void log(Level level, const char* file, int line, const char* format, ...);

    static Logger& instance() {
        static Logger logger;
        return logger;
    }

    void open(const std::string&);
    void close();
private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger operator=(const Logger&) = delete;

private:
    std::string filename_;
    std::ofstream fout_;
    static const String levels_[LEVEL_COUNT];
};


class _Logger final{
public:

private:
    const int flushInterval_;
    std::atomic<bool> running_;
    const String basename_;
    const off_t rollSize_;

    std::mutex mtx_;
    std::condition_variable cond_;



};

#define LOG
}


#endif //TUNDRA_LOGGING_H
