//
// Created by scgao on 2023/2/23.
//

#ifndef TUNDRA_BLOCKINGQUEUE_H
#define TUNDRA_BLOCKINGQUEUE_H

#include <mutex>
#include <condition_variable>
#include <deque>

#include <assert.h>


namespace tundra {

template<typename T>
class BlockingQueue final  {
public:
    using queue_type = std::deque<T>;

    explicit BlockingQueue(size_t maxCapacity = 1024) : capacity_(maxCapacity) { };
    ~BlockingQueue() = default;

    void push(const T& x) {
        std::unique_lock<std::mutex> ulk(mtx_);
        cond_full_.wait(ulk,[this](){return queue_.size() < capacity_;});
        assert(queue_.size() < capacity_);

        queue_.push_back(x);
        cond_empty_.notify_all();
    }

    void push(T&& x) {
        std::unique_lock<std::mutex> ulk(mtx_);
        cond_full_.wait(ulk,[this](){return queue_.size() < capacity_;});
        assert(queue_.size() < capacity_);

        queue_.push_back(std::move(x));
        cond_empty_.notify_all();
    }

    T take() {
        std::unique_lock<std::mutex> ulk(mtx_);
        cond_empty_.wait(ulk, [this](){return !this->queue_.empty();});
        assert(!queue_.empty());

        T element(std::move(queue_.front()));
        queue_.pop_front();
        cond_full_.notify_all();

        return element;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lg(mtx_);
        return queue_.empty();
    }

    bool full() const {
        std::lock_guard<std::mutex> lg(mtx_);
        return queue_.size() >= capacity_;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lg(mtx_);  //可能有其他的写者，所以这个读操作要上锁
        return queue_.size();
    }

    size_t capacity() const {
        std::lock_guard<std::mutex> lg(mtx_);
        return capacity_;
    }


private:
    mutable std::mutex mtx_;
    std::condition_variable cond_empty_;
    std::condition_variable cond_full_;
    queue_type queue_;
    size_t capacity_;
};

}

#endif //TUNDRA_BLOCKINGQUEUE_H
