#pragma once
#include <string.h>
#include <string>

#include "noncopyable.h"

class AsyncLogging;
constexpr int kSmallBufferSize = 4096;      // 4KB
constexpr int kLargeBufferSize = 4096 * 1000;

template <int buffer_size>
class FixedBuffer : noncopyable
{
public:
    FixedBuffer()
        : cur_(data_), size_(0)
    {
    }

    void append(const char *buf, size_t len)
    {
        if (avail() > len)
        {
            memcpy(cur_, buf, len); // 复制数据到缓冲区
            add(len);
        }
    }

    const char *data() const { return data_; }
    int length() const { return size_; }
    char *current() { return cur_; }
    size_t avail() const { return static_cast<size_t>(buffer_size - size_); }

    void add(size_t len)
    {
        cur_ += len;
        size_ += len;
    }

    void reset()
    {
        cur_ = data_;
        size_ = 0;
    }

    void bzero() { ::bzero(data_, sizeof(data_)); }

    std::string toString() const { return std::string(data_, length()); }    

private:
    char data_[buffer_size]; // 定义固定大小的缓冲区
    char *cur_;              // 当前指针，指向缓冲区中下一个可写入的位置
    int size_;               // 缓冲区的大小

};