//
// Created by scgao on 2023/3/17.
//

#include "HttpServer.h"
#include "base/Logging.h"
#include "http/HttpContext.h"
#include "http/HttpRequest.h"
#include "http/HttpResponse.h"

#include <sstream>

namespace tundra {
namespace http {
namespace detail {
    void defaultHttpCallback(const HttpRequest&, HttpResponse* resp) {
        resp->setStatusCode(HttpResponse::k404NotFound);
        resp->setStatusMessage("Not Found");
        resp->setCloseConnection(true);
    }
}

HttpServer::HttpServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const std::string &name,
                       TcpServer::OPtion option)
    : server_(loop, listenAddr, name, option),
    httpCallback_(detail::defaultHttpCallback)
{
//    server_.setConnectionCallback(
//            std::bind(&HttpServer::onConnection, this, _1));
    server_.setMessageCallback(
            std::bind(&HttpServer::onMessage, this, _1, _2, _3));
}

void HttpServer::start() {
    std::ostringstream logStream;
    logStream << "HttpServer[" << server_.name()
              << "] starts listening on " << server_.ipPort();
    Logging::instance().log_warn(logStream.str());
    server_.start();
}

void HttpServer::onConnection(const tundra::TcpConnectionPtr &conn) {

}

void HttpServer::onMessage(const tundra::TcpConnectionPtr &conn, tundra::Buffer *buf, tundra::TimeStamp receiveTime) {
    HttpContext context;

    if (!context.parseRequest(buf, receiveTime)) {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }

    if (context.gotAll()) {
        onRequest(conn, context.request());
    }
}

void HttpServer::onRequest(const tundra::TcpConnectionPtr& conn, const HttpRequest& req) {
    const std::string& connection = req.getHeader("Connection");
    bool close = connection == "close" ||
            (req.version() == HttpRequest::kHttp10 && connection != "Keep-Alive");
    HttpResponse response(close);
    httpCallback_(req, &response);
    tundra::Buffer buf;
    response.appendToBuffer(&buf);
    conn->send(&buf);
    if (response.closeConnection()) {
        conn->shutdown();
    }
}



}
}
