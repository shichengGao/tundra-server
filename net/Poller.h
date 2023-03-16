//
// Created by scgao on 2023/3/7.
//

#ifndef TUNDRA_POLLER_H
#define TUNDRA_POLLER_H
#include <vector>
#include <map>

#include "util/TimeStamp.h"
#include "net/EventLoop.h"

struct pollfd;

namespace tundra{
class Channel;

class Poller {
public:
    using ChannelList = std::vector<Channel*>;
    Poller(EventLoop* loop);
    ~Poller();

    Poller(const Poller&) = delete;
    Poller operator=(const Poller&) = delete;

    ///Polls the I/O events.
    ///Must be called in the loop thread.
    TimeStamp poll(int timeoutMs, ChannelList* activeChannels);

    ///Changes the interested I/O events.
    ///Must be called in the loop thread.
    void updateChannel(Channel* channel);

    void removeChannel(Channel* channel);

    bool hasChannel(Channel* channel) const;

    void assertInLoopThread() const {
        ownerLoop_->assertInLoopThread();
    }

private:
    //hand over events to Channels
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

    using PollFdList = std::vector<struct pollfd>;
    using ChannelMap = std::map<int, Channel*>;

    EventLoop* ownerLoop_;
    PollFdList pollfds_;
    ChannelMap channels_;
};

}


#endif //TUNDRA_POLLER_H
