//
// Created by scgao on 2023/3/7.
//

#ifndef TUNDRA_CHANNEL_H
#define TUNDRA_CHANNEL_H

#include <functional>
#include <string>
#include <memory>
#include "util/TimeStamp.h"

namespace tundra{

class EventLoop;

class Channel {
public:
    using EventCallback = std::function<void()>;
    using ReadeventCallback = std::function<void(TimeStamp)>;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    Channel(const Channel&) = delete;
    Channel operator=(const Channel&) = delete;

    void handleEvent(TimeStamp receiveTime);
    void setReadCallback(ReadeventCallback cb) {
        readCallback_ = std::move(cb);
    }
    void setWriteCallback(EventCallback cb) {
        writeCallback_ = std::move(cb);
    }
    void setCloseCallback(EventCallback cb) {
        closeCallback_ = std::move(cb);
    }
    void setErrorCallback(EventCallback cb) {
        errorCallback_ = std::move(cb);
    }

    ///prevent the owner object being destroyed in handleEvent.
    void tie(const std::shared_ptr<void>&);

    int fd() const {
        return fd_;
    }

    int events() const {
        return events_;
    }

    void set_revents(int revt) {
        revents_ = revt;
    }

    bool isNoneEvent() const {
        return events_ == kNoneEvent;
    }

    void enableReading() { events_ |= kReadEvent; update();}
    void disableReading() { events_ &= ~kReadEvent; update();}
    void enableWriting() { events_ |= kWriteEvent; update();}
    void disableWriting() { events_ &= ~kWriteEvent; update();}
    void disableAll() { events_ = kNoneEvent; update();}
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const { return events_ & kReadEvent; }

    //for poller
    int index(){ return index_; }
    void set_index(int idx) { index_ = idx; }

    //for debug
    std::string reventsToString() const;
    std::string eventsToString() const;

    void doNotLogHup() { logHup_ = false;}

    EventLoop* ownerLoop() { return loop_; }
    void remove();

private:
    static std::string eventsToString(int fd, int ev);

    void update();
    void handleEventWithGuard(TimeStamp receiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;
    const int fd_;
    int events_;  //IO events that we cared
    int revents_; //active events

    int index_; //index of pollfd in listend list
    bool logHup_;

    std::weak_ptr<void> tie_;
    bool tied_;
    bool eventHandling_;
    bool addedToLoop_;

    ReadeventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};

}


#endif //TUNDRA_CHANNEL_H
