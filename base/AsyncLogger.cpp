//
// Created by scgao on 2023/3/2.
//

#include "AsyncLogger.h"
#include "util/TimeStamp.h"
#include <assert.h>

namespace tundra {

const size_t AsyncLogger::blockSize_ = 4 * 1024 * 1024;

AsyncLogger::AsyncLogger(const std::string &basename, off_t rollSize, int flushInterval)
    :basename_(basename), rollSize_(rollSize), flushInterval_(flushInterval),
     stop_(true), latch_(1), curBufferCounts_(initialBufferCounts),
     currentBuffer_(std::make_unique<Buffer>(AsyncLogger::blockSize_))
{
    for (int i=0; i<initialBufferCounts-1; ++i)
        emptyBuffers_.emplace_back(std::make_unique<Buffer>(AsyncLogger::blockSize_));
}

void AsyncLogger::updateCurBuffer() {

    fullBuffers_.emplace_back(std::move(currentBuffer_));

    if (!emptyBuffers_.empty()) {       //get old buffer from empty buffers
        currentBuffer_ = std::move(emptyBuffers_.front());
        emptyBuffers_.pop_front();
    } else {                            //apply new buffer, buffer-nums may exceed the upper limits
        currentBuffer_ = std::make_unique<Buffer>(AsyncLogger::blockSize_);
        ++curBufferCounts_;
    }
}

void AsyncLogger::append(const char *logline, int len) {

    auto trueLogLine = TimeStamp::now().toFormatString() + " " + logline + "\n";
    std::lock_guard<std::mutex> lg(mtx_);
    if (currentBuffer_->avail() < len) {
//        printf("not enough space. update buffer.\n");
        updateCurBuffer();
        currentBuffer_->append(trueLogLine.c_str(), trueLogLine.size());
        if (fullBuffers_.size() > curBufferCounts_ / 2)
            cond_.notify_one();
    } else {
        currentBuffer_->append(trueLogLine.c_str(), trueLogLine.size());
    }
}

// 局部性原理
//TODO: 从emptyBuffers中加入Buffer时，是不是存在这样的情况：
//    按照栈使用emptyBuffers，新获取的空间就是之前用过的空间，它可能还在Cache中
//    如果按照队列使用emptyBuffers, 总是均匀的让每个buffer都被使用，反而导致Cache命中率低？


void AsyncLogger::run() {
    assert(!stop_);
    latch_.countDown();

    LogFile logFile(basename_, rollSize_);

    BufferVector buffersToWrite;

    while(!stop_) {
        assert(buffersToWrite.empty());

        {
            std::unique_lock<std::mutex> ulk(mtx_);
            if (fullBuffers_.empty()) {
                cond_.wait_for(ulk, std::chrono::seconds(flushInterval_));
//                printf("wake up or 3 seconds passed.\n");
            }

//            printf("empty nums: %d\n", emptyBuffers_.size());
//            printf("full nums: %d\n", fullBuffers_.size());
//            printf("curent usage: %d\n", currentBuffer_->size());

            if (fullBuffers_.empty())
                updateCurBuffer();  //log in time

            buffersToWrite.swap(fullBuffers_);



        }

        assert(!buffersToWrite.empty());

        if (buffersToWrite.size() > maxBufferCounts - 1) {
            char buf[256];
            snprintf(buf, sizeof(buf), "Dropped log message at %s, %zd buffers\n",
                     TimeStamp::now().toFormatString().c_str(), buffersToWrite.size()+1);
//            printf("%s\n", buf);
            fputs(buf,stderr);
            //TODO: output error logs
            buffersToWrite.erase(buffersToWrite.begin() + maxBufferCounts-1);
            logFile.append(buf, sizeof(buf));
        }

        for (const auto& buffer : buffersToWrite) {
            logFile.append(buffer->data_, buffer->size());
            buffer->clear();
        }

        //move buffers to empty logs
        {
            std::lock_guard<std::mutex> lg(mtx_);
            while (!buffersToWrite.empty()) {
                emptyBuffers_.emplace_front(std::move(buffersToWrite.front()));
                buffersToWrite.pop_front();
            }
//            printf("fill emptyBuffers\n");
        }

        assert(buffersToWrite.empty());

    }
}

}
