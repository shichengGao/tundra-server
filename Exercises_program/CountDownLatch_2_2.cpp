//
// Created by 郜事成 on 2023/2/8.
//
#include <vector>
#include <mutex>
#include <condition_variable>
/*
/ 条件变量的教科书级别应用
*/
//!
//! 这是对unique_lock的错误使用，不能在构造函数初始化ulk，它也是RAII类，构造导致上锁
//!
class CountDownLatch {
public:
    explicit CountDownLatch(int count) : ulk(mtx_), count_(count) { };
    void wait();
    void countDown();

private:
    mutable std::mutex mtx_;
    std::unique_lock<std::mutex> ulk;
    std::condition_variable cond_;
    int count_;
};

void CountDownLatch::wait() {
    cond_.wait(ulk, [this](){return count_ <= 0;});
}

void CountDownLatch::countDown() {
    std::lock_guard<std::mutex> lg(mtx_);
    --count_;
    if (count_ == 0)
        cond_.notify_all();
}