//
// Created by scgao on 2023/3/15.
//

#include "ThreadPool.h"
#include <assert.h>
#include <thread>

namespace tundra {


ThreadPool::ThreadPool(const std::string &name, int numThreads, size_t maxQueueSize)
    : name_(name), maxThreadNums(numThreads),
      maxQueueSize_(maxQueueSize), running_(false),
      taskQueue(maxQueueSize)
    {
        assert(numThreads > 0);
        assert(maxQueueSize >= numThreads);
    }

ThreadPool::~ThreadPool() {
    if (running_) {
        stop();
    }
}

void ThreadPool::start() {
    assert(threads_.empty());
    running_ = true;

    for (int i=0; i<maxThreadNums; ++i) {
        threads_.emplace_back(std::thread(&ThreadPool::threadFunc, this));
    }
}

void ThreadPool::stop() {
    running_ = false;

//    wakeup threads that no task to do.
    auto emptyFunc = [](){

    };

    for (int i=0; i < maxThreadNums; ++i) {
        taskQueue.push(emptyFunc);
    }

    for (auto& thread : threads_)
        thread.join();
}

void ThreadPool::enQueue(Task &&task) {
    if (running_) {
        taskQueue.push(task);
    }
}

//
//void ThreadPool::enQueue(Task&& task) {
//    std::unique_lock<std::mutex> ulk(mtx_);
//    //wakeup when taskqueue not full or threadPoll stopped.
//    notFull_.wait(ulk, [this](){return !running_ || !isFull();});
//
//    if (!running_) return;
//    assert(!isFull());
//
//    queue_.push_back(std::move(task));
//    notEmpty_.notify_one();
//}

//ThreadPool::Task ThreadPool::take() {
//    std::unique_lock<std::mutex> ulk(mtx_);
//    notEmpty_.wait(ulk, [this](){return !running_ || !queue_.empty();});
//
//    //when threadPool is stopped, also runs remaining tasks.
//    Task task;
//    if ()
//
//}

//bool ThreadPool::isFull() const {
//    return maxQueueSize_  == queue_.size();
//}

void ThreadPool::threadFunc() {
    if (threadInitCallback_) {
        threadInitCallback_();
    }

    while (running_ || !taskQueue.empty()) {
        Task task = taskQueue.take();
        task();
    }

}





}
