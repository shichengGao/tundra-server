//
// Created by scgao on 2023/2/25.
//

#ifndef TUNDRA_STRING_H
#define TUNDRA_STRING_H

#include <cstring>
#include <iosfwd>
#include <string>
#include <assert.h>

namespace tundra {
//only readable interface
class String {
public:
    String() : data_(""), len_(0) { }

    String(const char* str) : data_(str), len_(strlen(data_)) { }

    String(const unsigned char* str)
        : data_(reinterpret_cast<const char*>(str)), len_(strlen(data_)) { }

    String(const std::string& str) : data_(str.c_str()), len_(str.size()) {  }

    String(const char* offset, int len) : data_(offset), len_(len) { }

    String(const String& rhs) {
        data_ = rhs.data_;
        len_ = rhs.len_;
    }

    String(String&& rhs) {
        data_ = rhs.data_;
        len_ = rhs.len_;
        rhs.data_ = nullptr;
    }

    String operator=(const String& rhs) {
        data_ = rhs.data_;
        len_ = rhs.len_;
    }

    String& operator=(String&& rhs) {
        data_ = rhs.data_;
        len_ = rhs.len_;
        rhs.data_ = nullptr;
        return *this;
    }

    void swap(String& rhs) {
        std::swap(*this, rhs);
    }

    const char* data() const {
        return data_;
    }

    const char* begin() const {
        return data_;
    }

    const char* end() const {
        return data_ + len_;
    }

    int size() const {
        return len_;
    }

    bool empty() const {
        return len_ == 0;
    }

    bool starts_with(const String& pattern) {
        return (this->len_ >= pattern.len_)
                && (memcmp(this->data_, pattern.data_, pattern.len_) == 0);
    }

    void clear() {
        data_ = "";
        len_ = 0;
    }

    void set(const char* str, size_t len) {
        data_ = str;
        len_ = len;
    }

    void set(const char* str) {
        set(str, strlen(str));
    }

    void set(const void* str, size_t len) {
        set(reinterpret_cast<const char*>(str), len);
    }

    char operator[](int i) const {
        assert(i < len_);
        return data_[i];
    }

    int compare(const String& rhs) const {
        return strcmp(this->data_, rhs.data_);
    }

    std::string to_string() const {
        return std::string (data_, len_);
    }

    //implicit conversion to std::string
    std::string string() {
        return to_string();
    }



private:
    const char* data_;
    size_t len_;
};


bool inline operator==(const String&  lhs, const String& rhs);
bool inline operator!=(const String&lhs, const String& rhs);
bool operator<(const String&lhs, const String& rhs);
bool operator<=(const String& lhs, const String& rhs);
bool operator>(const String& lhs, const String& rhs);
bool operator>=(const String& lhs, const String& rhs);
} // tundra

#endif //TUNDRA_STRING_H
