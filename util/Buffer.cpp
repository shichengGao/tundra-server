//
// Created by scgao on 2023/2/25.
//

#include "Buffer.h"
#include <string>
#include <algorithm>
#include <vector>
#include <memory>

namespace tundra {

void Buffer::makeSpace(size_t len) {
    if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
        //FIXME: move readable data?

        buffer_.resize(writerIndex_ + len);
    } else {
        //moving data to the front of memory
        assert(kCheapPrepend < readerIndex_);
        size_t readable = readableBytes();

        std::copy(begin() + readerIndex_,
                  begin() + writerIndex_,
                  begin() + kCheapPrepend);

        readerIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend + readable;

        assert(readable == readableBytes());
    }

}

//peek data funcs
int8_t inline Buffer::peekInt8() const {
    assert(readableBytes() >= sizeof(int8_t));
    int8_t element = 0;
    ::memcpy(&element, peek(), sizeof(int8_t));
    return element;
}

int16_t inline Buffer::peekInt16() const {
    assert(readableBytes() >= sizeof(int16_t));
    int8_t element = 0;
    ::memcpy(&element, peek(), sizeof(int16_t));
    return element;
}

int32_t inline Buffer::peekInt32() const {
    assert(readableBytes() >= sizeof(int32_t));
    int8_t element = 0;
    ::memcpy(&element, peek(), sizeof(int32_t));
    return element;
}

int64_t inline Buffer::peekInt64() const {
    assert(readableBytes() >= sizeof(int64_t));
    int8_t element = 0;
    ::memcpy(&element, peek(), sizeof(int64_t));
    return element;
}

//retrieve funcs
void inline Buffer::retrieveInt8() {
    retrieve(sizeof(int8_t));
}

void inline Buffer::retrieveInt16() {
    retrieve(sizeof(int8_t));
}

void inline Buffer::retrieveInt32() {
    retrieve(sizeof(int8_t));
}

void inline Buffer::retrieveInt64() {
    retrieve(sizeof(int8_t));
}

//append funcs
void inline Buffer::appendInt8(int8_t element) {
        append(&element, sizeof(int8_t));
}

void inline Buffer::appendInt16(int16_t element) {
    append(&element, sizeof(int16_t));
}

void inline Buffer::appendInt32(int32_t element) {
    append(&element, sizeof(int32_t));
}

void inline Buffer::appendInt64(int64_t element) {
    append(&element, sizeof(int64_t));
}

//prepend funcs
void inline Buffer::prependInt8(int8_t element) {
    prepend(&element, sizeof(int8_t));
}

void inline Buffer::prependInt16(int16_t element) {
    prepend(&element, sizeof(int16_t));
}

void inline Buffer::prependInt32(int32_t element) {
    prepend(&element, sizeof(int32_t));
}

void inline Buffer::prependInt64(int64_t element) {
    prepend(&element, sizeof(int64_t));
}


//read funcs
int8_t inline Buffer::readInt8() {
    int8_t ret = peekInt8();
    retrieveInt8();
    return ret;
}

int16_t inline Buffer::readInt16() {
    int16_t ret = peekInt16();
    retrieveInt16();
    return ret;
}

int32_t inline Buffer::readInt32() {
    int32_t ret = peekInt32();
    retrieveInt32();
    return ret;
}

int64_t inline Buffer::readInt64() {
    int64_t ret = peekInt64();
    retrieveInt64();
    return ret;
}


} // tundra