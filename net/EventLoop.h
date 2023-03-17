//
// Created by scgao on 2023/3/4.
//

#ifndef TUNDRA_EVENTLOOP_H
#define TUNDRA_EVENTLOOP_H
#include <atomic>
#include <thread>
#include <mutex>
#include <vector>

#include "util/TimeStamp.h"
#include "util/types.h"
#include "net/TimerId.h"

namespace tundra {

class Channel;
class Poller;
class TimerQueue;

//one loop per thread
class EventLoop {
public:

    using Functor = std::function<void()>;

    EventLoop();
    ~EventLoop();

    EventLoop(const EventLoop&) = delete;
    EventLoop operator=(const EventLoop&) = delete;

    void loop();
    void quit();

    TimeStamp pollReturnTime() const {
        return pollReturnTime_;
    }

    int64_t iteration() const {
        return iteration_;
    }

    // internal usage
    void wakeup();
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);

    void assertInLoopThread() {
        if (!isInLoopThread()) {
            abortNotInLoopThread();
        }
    }

    bool isInLoopThread() const {
        return threadId_ == std::this_thread::get_id();
    }

    static EventLoop* getEventLoopOfCurrentThread();

    //timers
    TimerId runAt(const TimeStamp& time, TimerCallback cb);

    TimerId runAfter(double delay, TimerCallback cb);

    TimerId runEvery(double interval, TimerCallback cb);

    //cancels the timer
    void cancel(TimerId timerId);

    void runInLoop(Functor cb);

    void queueInLoop(Functor cb);

    size_t queueSize() const;


private:
    void abortNotInLoopThread();
    void handleRead(); //wake up
    void doPendingFunctors();

    void printActiveChannels() const;

    using ChannelList = std::vector<Channel*>;

    std::atomic<bool> looping_;
    std::atomic<bool> quit_;
    std::atomic<bool> eventHandling_;
    std::atomic<bool> callingPendingFunctors_;

    int64_t iteration_;
    const std::thread::id threadId_;
    TimeStamp pollReturnTime_;
    std::unique_ptr<Poller> poller_;
    std::unique_ptr<TimerQueue> timerQueue_;

    int wakeupFd_;
    //handle readable events of wakeupFd_
    std::unique_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;
    Channel* currentActiveChannel_;

    mutable std::mutex mtx_;
    std::vector<Functor> pendingFunctors_ __attribute__((guarded_by(mtx_)));

};

}


#endif //TUNDRA_EVENTLOOP_H
