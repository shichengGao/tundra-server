//
// Created by 郜事成 on 2023/2/10.
//
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include <functional>

template <typename T>
class BlockingQueue{
public:
    BlockingQueue() : ulk_(mtx_) { }

    void Enqueue(T&& element) {
        cond_.wait(ulk_, [this](){return data_.size() < capacity;});
        data_.push(element);
        cond_.notify_all();
    }

    T Dequeue() {           //???
        cond_.wait(ulk_, [this](){return data_.size() < capacity;});
        T ret = data_.front();
        data_.pop();
        cond_.notify_all();
    }



private:
    std::mutex mtx_;
    std::unique_lock<std::mutex> ulk_;
    std::condition_variable cond_;
    std::queue<T> data_;
    int capacity;
};