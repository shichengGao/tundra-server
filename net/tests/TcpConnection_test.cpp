//
// Created by scgao on 2023/3/14.
//

#include "net/TcpConnection.h"
#include "gtest/gtest.h"

using namespace std;
using namespace tundra;

TEST(TcpConnection_function_test, simpleTest) {
    auto onMessage = [](TcpConnectionPtr& conn,
            Buffer* buf, TimeStamp receiveTime) {
        printf("on Message(): received %zd bytes from connection [%s] at %s\n",
               buf->readableBytes(),
               conn->name().c_str(),
               receiveTime.toFormatString().c_str());

        printf("onMessage(): [%s]\n", buf->retrieveAllAsString().data());
    };


}
