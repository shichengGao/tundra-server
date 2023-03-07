//
// Created by scgao on 2023/3/4.
//

#ifndef TUNDRA_EVENTLOOP_H
#define TUNDRA_EVENTLOOP_H
#include <atomic>
#include <thread>
#include <vector>

namespace tundra {

class Channel;
class Poller;
class TimerQueue;

//one loop per thread
class EventLoop {
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    void updateChannel(Channel* channel);

    void assertInLoopThread() {
        if (!isInLoopThread()) {
            abortNotInLoopThread();
        }
    }

    bool isInLoopThread() const {
        return threadId_ == std::this_thread::get_id();
    }

    static EventLoop* getEventLoopOfCurrentThread();

private:
    void abortNotInLoopThread();

    using ChannelList = std::vector<Channel*>;

    std::atomic<bool> looping_;
    std::atomic<bool> quit_;
    const std::thread::id threadId_;
    std::unique_ptr<Poller> poller_;
    ChannelList activeChannels_;


};

}


#endif //TUNDRA_EVENTLOOP_H
