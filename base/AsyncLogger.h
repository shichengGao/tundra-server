//
// Created by scgao on 2023/3/2.
//

#ifndef TUNDRA_ASYNCLOGGER_H
#define TUNDRA_ASYNCLOGGER_H
#include <string>
#include <cstring>
#include <deque>
#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
#include <condition_variable>

#include "util/CountDownLatch.h"
#include "base/LogFile.h"

namespace tundra{

//an RAII class
class FixedBuffer {
public:
    FixedBuffer(size_t capacity) : capacity_(capacity),
            data_(new char[capacity]), cur_(data_) {
        bzero();
    }

    ~FixedBuffer() {
        delete data_;
    }

    void append(const std::string& msg) {
        append(msg.c_str(), msg.size());
    }

    void append(const char* buf, size_t len) {
        if (avail() > len) {
            memcpy(cur_, buf, len);
            cur_ += len;
        }
    }

    void bzero() {
        memset(data_, 0, capacity_);
    }

    void clear() {
        bzero();
        cur_ = data_;
    }

    size_t size() const {
        return cur_ - data_;
    }

    size_t capacity() const {
        return capacity_;
    }

    size_t avail() const {
        return capacity() - size();
    }

    friend class AsyncLogger;
private:
    char* data_;
    char* cur_;
    size_t capacity_;
};


class AsyncLogger final{
public:
    //TODO: add level to log contents
    enum Level {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        LEVEL_COUNT
    };

    static const size_t blockSize_; //4M per buffer
    static const int initialBufferCounts = 15;
    static const int maxBufferCounts = 16; //soft limit

    AsyncLogger(const std::string& basename,
                off_t rollSize = LogFile::defaultRollSize,
                int flushInterval = 3);

    ~AsyncLogger() {
        if (!stop_) {
            stop();
        }
    }


    void append(const char* logline, int len);

    void append(const std::string& msg) {
        append(msg.c_str(), msg.size());
    }

    void log_trace(const std::string& msg) {
        if (logLevel_ <= Level::TRACE) {
            append(msg);
        }
    }

    void log_debug(const std::string& msg) {
        if (logLevel_ <= Level::DEBUG) {
            append(msg);
        }
    }

    void log_info(const std::string& msg) {
        if (logLevel_ <= Level::INFO) {
            append(msg);
        }
    }

    void log_warn(const std::string& msg) {
        if (logLevel_ <= Level::WARN) {
            append(msg);
        }
    }

    void log_error(const std::string& msg) {
        if (logLevel_ <= Level::ERROR) {
            append(msg);
        }
    }

    void log_fatal(const std::string& msg) {
        if (logLevel_ <= Level::FATAL) {
            append(msg);
        }
    }

    void setLevel(Level level) {
        logLevel_ = level;
    }

    Level getLevel() const {
        return logLevel_;
    }

    void start() {
        stop_ = false;
        thread_ = std::thread(&AsyncLogger::run, this);
        latch_.wait();
    }

    void stop() {
        stop_ = true;
        cond_.notify_all();
        thread_.join();
    }


private:
    using Buffer = FixedBuffer;
    using BufferPtr = std::unique_ptr<FixedBuffer>;
    using BufferVector = std::deque<BufferPtr>;


    void run();  //background thread functions

    void updateCurBuffer();

    const int flushInterval_;
    std::atomic<bool> stop_;
    const std::string basename_;
    const off_t rollSize_;

    std::mutex mtx_;
    std::condition_variable cond_;
    tundra::CountDownLatch latch_;

    std::thread thread_;

    BufferPtr currentBuffer_;
    BufferVector emptyBuffers_;
    BufferVector fullBuffers_;

    int curBufferCounts_;

    std::atomic<Level> logLevel_;
};

}



#endif //TUNDRA_ASYNCLOGGER_H
