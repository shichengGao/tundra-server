//
// Created by scgao on 2023/3/15.
//

#ifndef TUNDRA_THREADPOOL_H
#define TUNDRA_THREADPOOL_H

#include "util/types.h"
#include "util/BlockingQueue.h"

#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <vector>
#include <deque>

namespace tundra {

//Deprecated
class ThreadPool {
public:
    using Task = std::function<void()>;

    explicit ThreadPool(const std::string& name = "ThreadPool", int numThreads = 1 , size_t maxQueueSize = 10);
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool operator=(const ThreadPool&) = delete;

    void setMaxSize(int maxSize) { maxQueueSize_ = maxSize;}
    void setThreadInitCallback(const Task& callback) {
        threadInitCallback_ = callback;
    }

    void start();
    void stop();

    const std::string& name() const {
        return name_;
    }

    size_t queueSize() const {
        std::lock_guard<std::mutex> lg(mtx_);
        return maxQueueSize_;
    }

    void enQueue(Task&& task);

private:
    bool isFull() const;
    void threadFunc();
    Task take();

//    mutable std::mutex mtx_;
//    std::condition_variable notEmpty_ __attribute__((guarded_by(mtx_)));
//    std::condition_variable notFull_ __attribute__((guarded_by(mtx_)));
    std::string name_;
    Task threadInitCallback_;
    std::vector<std::thread> threads_;
    BlockingQueue<Task> taskQueue;
    //    std::deque<Task> queue_ __attribute__((guarded_by(mtx_)));
    size_t maxQueueSize_;
    int maxThreadNums;
    std::atomic<bool> running_;
};

}


#endif //TUNDRA_THREADPOOL_H
