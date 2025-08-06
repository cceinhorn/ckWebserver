#pragma once

#include <cstddef>
#include <vector>
#include <string>
#include <assert.h>
#include <sys/types.h>
#include <iostream>
#include <algorithm>

/*  prependable     readable       write*/
class Buffer
{
public:
    static const size_t kCheapPrepend = 8;      // 粘包  
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize)
        : buffer_(kCheapPrepend + initialSize)
        , readerIndex_(kCheapPrepend)
        , writerIndex_(kCheapPrepend)
    {}

    size_t readableBytes() const { return writerIndex_ - readerIndex_; }
    size_t writableBytes() const { return buffer_.size() - writerIndex_; }
    size_t prependableBytes() const { return readerIndex_; }

    const char *peek() const { return begin() + readerIndex_; }

    // 读取完 复位
    void retrieve(size_t len);
    void retrieveAll();

    // 把onMessage函数上报的Buffer数据 转成string类型的数据返回
    std::string retrieveAllAsString();
    std::string retrieveAsString(size_t len);
    // buffer_.size - writerIndex_
    void ensureWritableBytes(size_t len);
    void append(const std::string& str) { append(str.data(), str.length()); }
    void append(const char *data, size_t len);

    char *beginWrite() { return begin() + writerIndex_; }
    const char* beginWrite() const { return begin() + writerIndex_; }
    
    ssize_t readFd(int fd, int *saveErrno);
    ssize_t writeFd(int fd, int *saveErrno);

    const char* findCRLF() const
    {
        // FIXME: replace with memmem()?
        const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF+2);
        return crlf == beginWrite() ? NULL : crlf;
    }

    void retrieveUntil(const char* end)
    {
        assert(peek() <= end);
        assert(end <= beginWrite());
        retrieve(end - peek());
    }

private:
    char *begin() { return &*buffer_.begin(); }
    const char *begin() const { return &*buffer_.begin(); }
    void makeSpace(size_t len);

    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;

    static const char kCRLF[];
};