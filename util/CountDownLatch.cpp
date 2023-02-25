//
// Created by scgao on 2023/2/25.
//

#include "CountDownLatch.h"

namespace tundra {
    CountDownLatch::CountDownLatch(int count)
    : count_(count) {

    }

    void CountDownLatch::wait() {
        std::unique_lock<std::mutex> ulk(mtx_);
        cond_.wait(ulk, [this](){return count_<=0;});
    }

    void CountDownLatch::countDown() {
        std::lock_guard<std::mutex> lg(mtx_);
        if (--count_ == 0)
            cond_.notify_all();
    }
}
