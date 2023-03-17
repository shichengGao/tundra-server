//
// Created by scgao on 2023/3/16.
//

#ifndef TUNDRA_HTTPREQUEST_H
#define TUNDRA_HTTPREQUEST_H

#include "util/TimeStamp.h"
#include <string>
#include <map>

namespace tundra{
namespace http{
class HttpRequest {
public:
    enum Method{
        kInvalid, kGet, kPost, kHead, kPut, kDelete
    };
    enum Version{
        kUnknown, kHttp10, kHttp11
    };

    HttpRequest() : method_(kInvalid), version_(kUnknown) { }

    void setVersion(Version v) {
        version_ = v;
    }

    Version version() const {
        return version_;
    }

    bool setMethod(const char* start, const char* end) {
        assert(method_ == kInvalid);
        std::string m(start, end);
        if (m == "GET") {
            method_ = kGet;
        } else if (m == "POST") {
            method_ = kPost;
        } else if (m == "HEAD") {
            method_ = kHead;
        } else if (m == "PUT") {
            method_ = kPut;
        } else if (m == "DELETE") {
            method_ = kDelete;
        } else {
            method_ = kInvalid;
        }
        return method_ != kInvalid;
    }

    Method method() const {
        return method_;
    }

    const char* methodString() const {
        const char* res = "UNKNOWN";
        switch (method_) {
            case kGet:
                res = "GET";
                break;
            case kPost:
                res = "POST";
                break;
            case  kHead:
                res = "HEAD";
                break;
            case kPut:
                res = "PUT";
                break;
            case kDelete:
                res = "DELETE";
                break;
            default:
                break;
        }
        return res;
    }

    void setPath(const char* start, const char* end) {
        path_.assign(start, end);
    }

    const std::string& path() const{
        return path_;
    }

    void setQuery(const char* start, const char* end) {
        query_.assign(start, end);
    }

    const std::string query() const {
        return query_;
    }

    void setReceiveTime(TimeStamp time) {
        receiveTime_ = time;
    }

    TimeStamp receiveTime() const {
        return receiveTime_;
    }

    void addHeader(const char* start, const char* colon, const char* end) {
        std::string field(start, colon);
        ++colon;
        while (colon < end && std::isspace(*colon)) {
            ++colon;
        }
        std::string value(colon, end);
        while (!value.empty() && std::isspace(value.back())) {
            value.resize(value.size()-1);
        }
        headers_[field] = value;
    }

    std::string getHeader(const std::string& field) const {
        std::string res;
        std::map<std::string, std::string>::const_iterator it = headers_.find(field);
        if (it != headers_.end()) {
            res = it->second;
        }
        return res;
    }

    const std::map<std::string, std::string>& headers() const {
        return headers_;
    }

    void swap(HttpRequest& rhs) {
        std::swap(method_, rhs.method_);
        std::swap(version_, rhs.version_);
        path_.swap(rhs.path_);
        query_.swap(rhs.query_);
        std::swap(this->receiveTime_, rhs.receiveTime_);
        headers_.swap(rhs.headers_);
    }

private:
    Method method_;
    Version version_;
    std::string path_;
    std::string query_;
    TimeStamp receiveTime_;
    std::map<std::string, std::string> headers_;
};

}
}




#endif //TUNDRA_HTTPREQUEST_H
