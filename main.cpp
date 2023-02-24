//
// Created by 郜事成 on 2023/2/18.
//

#include <iostream>
#include <memory>
#include "BlockingQueue.h"
#include <unistd.h>
#include <stdlib.h>
#include <chrono>
#include <atomic>
#include <thread>
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
            this_thread::sleep_for(std::chrono::milliseconds(500 + rand() % 1000));

        }
    };

    auto consumer = [buffer, &stop]() {
        while (true) {
            if(stop.load(std::memory_order_relaxed)) {
                printf("consumer quit\n");
                break;
            }
            auto entry = buffer->pop();
            this_thread::sleep_for(std::chrono::milliseconds(500 + rand() % 1000));

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
            printf("size  : %d\n", buffer->size());
            this_thread::sleep_for(std::chrono::seconds(1));

        }
    };

    vector<std::thread> threads;
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


int main(int argc,char* argv[]) {
    runProducerConsumer();
}
