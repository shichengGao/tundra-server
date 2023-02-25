//
// Created by scgao on 2023/2/25.
//

#ifndef TUNDRA_COUNTDOWNLATCH_H
#define TUNDRA_COUNTDOWNLATCH_H

#include <vector>
#include <mutex>
#include <condition_variable>

namespace tundra{

class CountDownLatch {
public:
    CountDownLatch(const CountDownLatch&) = delete;
    CountDownLatch operator=(const CountDownLatch&) = delete;

    explicit CountDownLatch(int count);
    void wait();
    void countDown();

private:
    mutable std::mutex mtx_;
    std::condition_variable cond_;
    int count_;
};

}

#endif //TUNDRA_COUNTDOWNLATCH_H
