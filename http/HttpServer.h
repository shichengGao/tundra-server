//
// Created by scgao on 2023/3/17.
//

#ifndef TUNDRA_HTTPSERVER_H
#define TUNDRA_HTTPSERVER_H

#include "net/TcpServer.h"

namespace tundra{
namespace http{

class HttpRequest;
class HttpResponse;

class HttpServer {
public:
    using HttpCallback = std::function<void(const HttpRequest&, HttpResponse*)>;

    HttpServer(EventLoop* loop,
               const InetAddress& listenAddr,
               const std::string& name,
               TcpServer::OPtion option = TcpServer::kNoReusePort);

    EventLoop* getLoop() const {
        return server_.getLoop();
    }

    void setHttpCallback(const HttpCallback& cb) {
        httpCallback_ = cb;
    }

    void setThreadNum(int numThreads) {
        server_.setThreadNum(numThreads);
    }

    void start();

private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn,
                   Buffer* buf, TimeStamp receiveTime);
    void onRequest(const TcpConnectionPtr&, const HttpRequest&);

    TcpServer server_;
    HttpCallback httpCallback_;
};

}
}


#endif //TUNDRA_HTTPSERVER_H
