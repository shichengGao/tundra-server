//
// Created by scgao on 2023/3/7.
//

#include "Poller.h"
#include "net/Channel.h"
#include "base/Logging.h"

#include <poll.h>
#include <sstream>



namespace tundra {

Poller::Poller(tundra::EventLoop *loop) :
    ownerLoop_(loop) { }

Poller::~Poller() { }

TimeStamp Poller::poll(int timeoutMs, tundra::Poller::ChannelList *activeChannels) {
    //poll is a blocking function
    int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);
    TimeStamp now(TimeStamp::now());

    if (numEvents > 0) {
        std::ostringstream ss;
        ss << numEvents << " events happened";
        Logging::instance().log_trace(ss.str());
        fillActiveChannels(numEvents, activeChannels);
    } else if (numEvents == 0) {
        Logging::instance().log_trace(" nothing happended");
    } else {
        Logging::instance().log_error("Poller::poll() error");
    }
    return now;
}

void Poller::fillActiveChannels(int numEvents, tundra::Poller::ChannelList *activeChannels) const {
    for (PollFdList::const_iterator pfd = pollfds_.begin();
        pfd != pollfds_.end() && numEvents > 0; ++pfd) {
        if (pfd->revents > 0) {
            --numEvents;
            ChannelMap::const_iterator ch = channels_.find(pfd->fd);
            assert(ch != channels_.end());
            Channel* channel = ch->second;
            assert(channel->fd() == pfd->fd);
            channel->set_revents(pfd->revents);
            activeChannels->push_back(channel);
        }
    }//for
}

void Poller::updateChannel(tundra::Channel *channel) {
    assertInLoopThread();

    std::ostringstream ss;
    ss << "fd = "<< channel->fd() <<" events = " << channel->events();
    Logging::instance().log_trace(ss.str());

    if (channel->index() < 0) {
        //add new one
        assert(channels_.find(channel->fd()) == channels_.end());
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        pollfds_.push_back(pfd);
        int idx = static_cast<int>(pollfds_.size()-1);
        channel->set_index(idx);
        channels_[pfd.fd] = channel;
    } else {
        //update existing one
        assert(channels_.find(channel->fd()) != channels_.end());
        assert(channels_[channel->fd()] == channel);
        int idx = channel->index();
        assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
        struct pollfd& pfd = pollfds_[idx];
        assert(pfd.fd == channel->fd() || pfd.fd == -1);
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        if (channel->isNoneEvent()) {
            //ignore the pollfd
            pfd.fd = -1;
        }
    }//if-else
}


}

