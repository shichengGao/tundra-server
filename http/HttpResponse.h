//
// Created by scgao on 2023/3/17.
//

#ifndef TUNDRA_HTTPRESPONSE_H
#define TUNDRA_HTTPRESPONSE_H

#include "util/types.h"
#include <map>

namespace tundra{
namespace http{
class Buffer;

class HttpResponse {
public:
    enum HttpStatusCode {
        kUnknown,
        k200OK = 200,
        k301MovePermanently = 301,
        k400BadRequest = 400,
        k404NotFound = 404,
    };

    explicit HttpResponse(bool close)
        : statusCode_(kUnknown), closeConnection_(close) { }

    void setStatusCode(HttpStatusCode code) {
        statusCode_ = code;
    }

    void setStatusMessage(const std::string& message) {
        statusMessage_ = message;
    }

    void setCloseConnection(bool on){
        closeConnection_ = on;
    }

    bool closeConnection() const {
        return closeConnection_;
    }

    void setContentType(const std::string& contentType) {
        addHeader("Content-Type", contentType);
    }

    void addHeader(const std::string& key, const std::string& value) {
        headers_[key] = value;
    }

    void setBody(const std::string& body) {
        body_ = body;
    }

    void appendToBuffer(tundra::Buffer* output) const;

private:
    std::map<std::string, std::string> headers_;
    HttpStatusCode statusCode_;

    std::string statusMessage_;
    bool closeConnection_;
    std::string body_;
};

}
}


#endif //TUNDRA_HTTPRESPONSE_H
