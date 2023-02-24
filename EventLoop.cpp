//
// Created by 郜事成 on 2023/2/23.
//

#include "EventLoop.h"

namespace tundra {
__thread EventLoop* t_loopInThisThread = 0;
EventLoop::EventLoop()
        :looping_(false),
        threadId_(CurrentThread::tid())
        {
        LOG_TRACE << "Event"
    }
} // tundra