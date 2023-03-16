//
// Created by scgao on 2023/3/7.
//

#include "Channel.h"
#include "net/EventLoop.h"
#include "util/TimeStamp.h"
#include "base/Logging.h"

#include <assert.h>
#include <poll.h>
#include <sstream>

namespace tundra{

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(tundra::EventLoop *loop, int fd)
    : loop_(loop), fd_(fd), events_(0),
    revents_(0), index_(-1),logHup_(true),
    tied_(false), eventHandling_(false), addedToLoop_(false) { }

Channel::~Channel() {
    assert(!eventHandling_);
    assert(!addedToLoop_);
    if (loop_->isInLoopThread()) {
        assert(!loop_->hasChannel(this));
    }
}

void Channel::tie(const std::shared_ptr<void>& obj) {
    tie_ = obj;
    tied_ = true;
}

void Channel::update() {
    addedToLoop_ = true;
    loop_->updateChannel(this);
}

void Channel::remove() {
    assert(isNoneEvent());
    addedToLoop_ = false;
    loop_->removeChannel(this);
}

void Channel::handleEvent(TimeStamp receiveTime) {
    std::shared_ptr<void> guard;
    if (tied_) {
        guard = tie_.lock();
        if (guard) {
            handleEventWithGuard(receiveTime);
        }
    } else {
        handleEventWithGuard(receiveTime);
    }
}

void Channel::handleEventWithGuard(TimeStamp receiveTime) {
    eventHandling_ = true;
    Logging::instance().log_trace(reventsToString());
    std::ostringstream logStream;

    if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
        if (logHup_) {
            logStream << "fd = " << fd_ <<" Channel::handle_event() POLLHUP";
            Logging::instance().log_warn(logStream.str());
        }
        if (closeCallback_)
            closeCallback_();
    }

    if (revents_ & POLLNVAL) {
        logStream.clear();
        logStream << "fd = " << fd_ <<" Channel::handle_event() POLLNVAL";
        Logging::instance().log_warn(logStream.str());
    }

    if (revents_ & (POLLERR | POLLNVAL)) {
        if (errorCallback_) errorCallback_();
    }
    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
        if (readCallback_) readCallback_(receiveTime);
    }
    if (revents_ & POLLOUT) {
        if (writeCallback_) writeCallback_();
    }
    eventHandling_ = false;
}

std::string Channel::reventsToString() const {
    return eventsToString(fd_, revents_);
}

std::string Channel::eventsToString() const {
    return eventsToString(fd_, events_);
}

std::string Channel::eventsToString(int fd, int ev) {
    std::ostringstream ss;
    ss << fd << ": ";
    if (ev & POLLIN)
        ss << "IN ";
    if (ev & POLLPRI)
        ss << "PRI ";
    if (ev & POLLOUT)
        ss << "OUT ";
    if (ev & POLLHUP)
        ss << "HUP ";
    if (ev & POLLRDHUP)
        ss << "RDHUP ";
    if (ev & POLLERR)
        ss << "ERR ";
    if (ev & POLLNVAL)
        ss << "NVAL";

    return ss.str();
}




}
