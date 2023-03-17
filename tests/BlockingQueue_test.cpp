//
// Created by 郜事成 on 2023/2/24.
//

#include "gtest/gtest.h"
#include "../util/BlockingQueue.h"
#include <memory>
#include <thread>
#include <chrono>
#include <vector>

using namespace std;

struct Entry{
public:
    Entry(int id = 0, double width = 18.0, bool isEmergency = false)
        : id(id), width(width), isEmgercy(isEmergency) {  };

    int id;
    double width;
    bool isEmgercy;
};

void runProducerConsumer() {
    auto buffer = std::make_shared<tundra::BlockingQueue<Entry>>(5);
    std::atomic<bool> stop(false);
    auto producer = [buffer, &stop](){
        printf("producer begin\n");
        int i = 0;
        while(true) {
            if(stop.load(std::memory_order_relaxed)) {
                printf("producer quit\n");
                break;
            }
            buffer->push(Entry(i++));
            std::this_thread::sleep_for(std::chrono::milliseconds(500 + rand() % 1000));

        }
    };

    auto consumer = [buffer, &stop]() {
        while (true) {
            if(stop.load(std::memory_order_relaxed)) {
                printf("consumer quit\n");
                break;
            }
            auto entry = buffer->take();
            std::this_thread::sleep_for(std::chrono::milliseconds(500 + rand() % 1000));

        }
    };

    auto monitor = [buffer, &stop]() {
        printf("monitor begin\n");
        while (true) {
            if(stop.load(std::memory_order_relaxed)) {
                printf("monitor quit\n");
                break;
            }
            printf("is Empty? : %s\n", buffer->empty()?"YES" : "NO");
            printf("is Full? : %s\n", buffer->full()? "YES" : "NO");
            printf("size  : %d\n", static_cast<int>(buffer->size()));
            std::this_thread::sleep_for(std::chrono::seconds(1));

        }
    };

    std::vector<std::thread> threads;
    for (int i=0; i<3; ++i) {
        threads.emplace_back(std::thread(producer));
    }
    sleep(5);

    for (int i=0; i<3; ++i) {
        threads.emplace_back(std::thread(consumer));
    }


    threads.emplace_back(std::thread(monitor));

    sleep(10);
    stop.store(true, std::memory_order_relaxed);
    printf("STOP SIGNAL!\n");

    for (auto& it : threads) {
        it.join();
    }
}

TEST(BlockingQueue_test, testProducerConsumer) {

    runProducerConsumer();
    EXPECT_TRUE(true); //能运行到这里就不算死锁

}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}