//
// Created by scgao on 2023/3/17.
//

#ifndef TUNDRA_HTTPCONTEXT_H
#define TUNDRA_HTTPCONTEXT_H

#include "http/HttpRequest.h"

namespace tundra{
class Buffer;
namespace http{

class HttpContext {
public:
    enum HttpRequestParseState{
        kExpectRequestLine,
        kExpectHeaders,
        kExpectody,
        kGotAll
    };

    HttpContext() : state_(kExpectRequestLine) { }

    bool parseRequest(Buffer* buf, TimeStamp receiveTime);

    bool gotAll() const {
        return state_ == kGotAll;
    }

    void reset() {
        state_ = kExpectRequestLine;
        HttpRequest dummy; //exception safety
        request_.swap(dummy);
    }

    const HttpRequest& request() const {
        return request_;
    }

    HttpRequest& request(){
        return request_;
    }

private:
    bool processRequestLine(const char* begin, const char* end);

    HttpRequestParseState state_;
    HttpRequest request_;
};

}
}


#endif //TUNDRA_HTTPCONTEXT_H
