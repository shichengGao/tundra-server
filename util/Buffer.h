//
// Created by scgao on 2023/2/25.
//

#ifndef TUNDRA_BUFFER_H
#define TUNDRA_BUFFER_H

#include <vector>
#include <string>
#include <assert.h>

using std::size_t;

namespace tundra {

class Buffer {
public:

    static const size_t kInitialSize = 4096;
    explicit Buffer(size_t initialSize = kInitialSize);

    void swap();
    size_t readableBytes() const {
        return writerIndex_ - readerIndex_;
    }

    size_t writableBytes() const {
        return buffer_.size() - writerIndex_;
    }

    const char* begin() const {
        return &(*buffer_.begin());
    }

    const char* peek() const {
        return begin() + readerIndex_;
    }

    void retrieveAll() {
        readerIndex_ = writerIndex_ = 0;
    }

    void retrieve(size_t len) {
        assert(len <= readableBytes());
        if (len < readableBytes()) {
            readerIndex_ += len;
        } else {
            retrieveAll();
        }
    }

    void retrieveUntil(const char* end) {
        assert(peek() <= end);
        retrieve(end-peek());
    }

    std::string retrieveAsString(size_t len) {
        assert(len <= readableBytes());
        retrieve(len);
        return std::string(begin(), len);
    }

    void append()



private:

    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;


};

} // tundra

#endif //TUNDRA_BUFFER_H
