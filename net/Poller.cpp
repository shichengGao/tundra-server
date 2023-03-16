//
// Created by scgao on 2023/3/7.
//

#include "Poller.h"
#include "net/Channel.h"
#include "base/Logging.h"
#include "util/TimeStamp.h"

#include <poll.h>
#include <assert.h>
#include <sstream>



namespace tundra {

Poller::Poller(tundra::EventLoop *loop) :
    ownerLoop_(loop) { }

Poller::~Poller() = default;

bool Poller::hasChannel(Channel* channel) const {
    assertInLoopThread();
    ChannelMap::const_iterator it = channels_.find(channel->fd());
    return it != channels_.end() && it->second == channel;
}

TimeStamp Poller::poll(int timeoutMs, ChannelList *activeChannels) {
    //poll is a blocking function
    int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);
    int savedErrno = errno;
    TimeStamp now(TimeStamp::now());

    if (numEvents > 0) {
        std::ostringstream logStream;
        logStream << numEvents << " events happened";
        Logging::instance().log_trace(logStream.str());
        fillActiveChannels(numEvents, activeChannels);
    } else if (numEvents == 0) {
        Logging::instance().log_trace(" nothing happended");
    } else {
        //Interrupted system call would not shut down the program.
        if (savedErrno != EINTR) {
            errno = savedErrno;
            Logging::instance().log_error("Poller::poll() error");
        }
    }
    return now;
}

void Poller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const {
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

    std::ostringstream logStream;
    logStream << "fd = "<< channel->fd() <<" events = " << channel->events();
    Logging::instance().log_trace(logStream.str());

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
        assert(pfd.fd == channel->fd() || pfd.fd == -channel->fd()-1);
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        if (channel->isNoneEvent()) {
            //ignore the pollfd
            pfd.fd = -channel->fd()-1;
        }
    }//if-else
}

void Poller::removeChannel(Channel *channel) {
    assertInLoopThread();
    std::ostringstream logStream;
    logStream << "fd = " << channel->fd();
    Logging::instance().log_trace(logStream.str());
    assert(channels_.find(channel->fd()) != channels_.end());
    assert(channels_[channel->fd()] == channel);
    assert(channel->isNoneEvent());

    int idx = channel->index();
    assert(0 <= idx && idx < pollfds_.size());
    const struct pollfd& pfd = pollfds_[idx];
    assert(pfd.fd == -channel->fd()-1 && pfd.events == channel->events());
    size_t n = channels_.erase(channel->fd());
    assert(n == 1);
    if (idx == pollfds_.size()-1) {
        pollfds_.pop_back();
    } else {
        int channelAtEnd = pollfds_.back().fd;
        std::iter_swap(pollfds_.begin()+idx, pollfds_.end()-1);
        if (channelAtEnd < 0) {
            channelAtEnd = -channelAtEnd-1;
        }
        channels_[channelAtEnd]->set_index(idx);
        pollfds_.pop_back();
    }
}


}


