//
// Created by scgao on 2023/3/7.
//

#ifndef TUNDRA_CHANNEL_H
#define TUNDRA_CHANNEL_H

#include <functional>

namespace tundra{

class EventLoop;

class Channel {
public:
    using EventCallback = std::function<void()>;
    Channel(EventLoop* loop, int fd);
    ~Channel();

    Channel(const Channel&) = delete;
    Channel operator=(const Channel&) = delete;

    void handleEvent();
    void setReadCallback(const EventCallback& cb) {
        readCallback_ = cb;
    }

    void setWriteCallback(const EventCallback& cb) {
        writeCallback_ = cb;
    }

    void setErrorCallback(const EventCallback& cb) {
        errorCallback_ = cb;
    }

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
    void enableWritting() { events_ |= kWriteEvent; update();}
    void disableWritting() { events_ &= ~kWriteEvent; update();}
    void disableAll() { events_ = kNoneEvent; update();}

    //for poller
    int index(){ return index_; }
    void set_index(int idx) { index_ = idx; }

    EventLoop* ownerLoop() { return loop_; }

    ///prevent the owner object being destroyed in handleEvent.
//    void tie(const std::shared_ptr<void>&);
//    void remove();

private:
    void update();

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;
    const int fd_;
    int events_;  //IO events that we cared
    int revents_; //active events

    int index_; //index of pollfd in listend list

    bool eventHandling_;
    bool addedToLoop_;



    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};

}


#endif //TUNDRA_CHANNEL_H
