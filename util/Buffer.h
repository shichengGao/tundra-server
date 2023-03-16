//
// Created by scgao on 2023/2/25.
//

#ifndef TUNDRA_BUFFER_H
#define TUNDRA_BUFFER_H

#include <vector>
#include <assert.h>

#include "String.h"

namespace tundra {

class Buffer {
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 4088;

    static const char kCRLF[];

    explicit Buffer(size_t initialSize = kInitialSize)
        : buffer_(kCheapPrepend + kInitialSize),
          readerIndex_(kCheapPrepend),
          writerIndex_(kCheapPrepend) {

        assert(readableBytes() == 0);
        assert(writableBytes() == initialSize);
    }

    void swap(Buffer& rhs) {
        buffer_.swap(rhs.buffer_);
        std::swap(readerIndex_, rhs.readerIndex_);
        std::swap(writerIndex_, rhs.writerIndex_);
    }

    void ensureWriteableBytes(size_t len) {
        if (writableBytes() < len) {
            makeSpace(len);
        }
        assert(writableBytes() >= len);
    }

    size_t readableBytes() const {
        return writerIndex_ - readerIndex_;
    }

    size_t writableBytes() const {
        return buffer_.size() - writerIndex_;
    }

    size_t prependableBytes() const {
        return readerIndex_;
    }

    const char* peek() const {
        return begin() + readerIndex_;
    }

    int8_t peekInt8 () const;
    int16_t peekInt16() const;
    int32_t peekInt32() const;
    int64_t peekInt64() const;


    char* beginWrite() {
        return begin() + writerIndex_;
    }

    const char* beginWrite() const {
        return begin() + writerIndex_;
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
        assert(end <= beginWrite());
        retrieve(end-peek());
    }

    String retrieveAsString(size_t len) {
        assert(len <= readableBytes());
        auto s = String(begin(), len);
        retrieve(len);
        return s;

    }

    String retrieveAllAsString() {
        retrieveAsString(readableBytes());
    }

    void retrieveInt8();
    void retrieveInt16();
    void retrieveInt32();
    void retrieveInt64();

    int8_t readInt8();
    int16_t readInt16();
    int32_t readInt32();
    int64_t readInt64();

    void append(const char* data, size_t len) {
        ensureWriteableBytes(len);
        std::copy(data, data + len, beginWrite());
        writerIndex_ += len;
    }

    void append(const void* data, size_t len) {
        append(reinterpret_cast<const char*>(data), len);
    }

    void append(const String& str) {
        append(str.data(), str.size());
    }

    void appendInt8(int8_t);
    void appendInt16(int16_t);
    void appendInt32(int32_t);
    void appendInt64(int64_t);


    void hasWritten(size_t len) {
        assert(len <= writableBytes());
        writerIndex_ += len;
    }

    void unwrite(size_t len) {
        assert(len < readableBytes());
        writerIndex_ -= len;
    }

    void prepend(const char* data, size_t len) {
        assert(prependableBytes() >= len);
        readerIndex_ -= len;
        std::copy(data, data+len,begin());
    }

    void prepend(const void* data, size_t len) {
        prepend(reinterpret_cast<const char*>(data), len);
    }

    void prependInt8(int8_t);
    void prependInt16(int16_t);
    void prependInt32(int32_t);
    void prependInt64(int64_t);

    void shrink(size_t reserve) {
        buffer_.shrink_to_fit();
    }

    size_t capacity() const {
        return buffer_.capacity();
    }

    ssize_t readFd(int fd, int* savedErrno);


private:
    void makeSpace(size_t len); //keep writable space by moving or extending

    char* begin(){
        return &(*buffer_.begin());
    }

    const char* begin() const {
        return &(*buffer_.begin());
    }

    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;


};

} // tundra

#endif //TUNDRA_BUFFER_H
