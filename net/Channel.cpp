//
// Created by scgao on 2023/3/7.
//

#include "Channel.h"
#include "net/EventLoop.h"
#include "base/Logging.h"

#include <poll.h>

namespace tundra{

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(tundra::EventLoop *loop, int fd)
    : loop_(loop), fd_(fd), events_(0),
    revents_(0), index_(-1) { }

void Channel::update() {
    loop_->updateChannel(this);
}

void Channel::handleEvent() {

    if (revents_ & POLLNVAL) {
        Logging::instance().log_warn("Channel::handle_event() POLLNVAL");
    }

    if (revents_ & (POLLERR | POLLNVAL)) {
        if (errorCallback_) errorCallback_();
    }
    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
        if (readCallback_) readCallback_();
    }
    if (revents_ & POLLOUT) {
        if (writeCallback_) writeCallback_();
    }
}


}
