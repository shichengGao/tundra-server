//
// Created by scgao on 2023/3/8.
//

#ifndef TUNDRA_TYPES_H
#define TUNDRA_TYPES_H

#include <functional>
#include <memory>
#include "util/TimeStamp.h"

namespace tundra{

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

class Buffer;
class TcpConnection;

using TcpConnectionPtr =  std::shared_ptr<TcpConnection>;

using TimerCallback = std::function<void()>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using CloseCallback = std::function<void(const TcpConnectionPtr&)>;
using MessageCallback = std::function<void(const TcpConnectionPtr&,
            Buffer*, TimeStamp receivedTime)>;

}


#endif //TUNDRA_TYPES_H
