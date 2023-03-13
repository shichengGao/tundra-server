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

    void loop();
    void quit();

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
    TimerId runAt(const TimeStamp& time, const TimerCallback& cb);

    TimerId runAfter(double delay, const TimerCallback& cb);

    TimerId runEvery(double interval, const TimerCallback& cb);


    void runInLoop(const Functor& cb);

    void queueInLoop(const Functor& cb);


private:
    void abortNotInLoopThread();
    void handleRead(); //wake up
    void doPendingFunctors();

    using ChannelList = std::vector<Channel*>;

    std::atomic<bool> looping_;
    std::atomic<bool> quit_;
    std::atomic<bool> callingPendingFunctors_;

    const std::thread::id threadId_;
    std::unique_ptr<Poller> poller_;
    std::unique_ptr<TimerQueue> timerQueue_;

    int weakupFd_;
    //handle readable events of wakeupFd_
    std::unique_ptr<Channel> wakeupChannel_;
    ChannelList activeChannels_;

    std::mutex mtx_;
    std::vector<Functor> pendingFunctors_;

};

}


#endif //TUNDRA_EVENTLOOP_H
