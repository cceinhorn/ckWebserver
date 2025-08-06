#pragma once

#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>

#include "noncopyable.h"
#include "Thread.h"
#include "FixedBuffer.h"
#include "LogStream.h"
#include "LogFile.h"


class AsyncLogging
{
public:
    AsyncLogging(const std::string &basename, off_t rollSize, int flushInterval=3);
    ~AsyncLogging()
    {
        if (running_)
        {
            stop();
        }
    }

    void append(const char *logline, int len);

    void start()
    {
        running_ = true;
        thread_.start();
    }
    void stop()
    {
        running_ = false;
        cond_.notify_one();
    }
    
private:
    using LargeBuffer = FixedBuffer<kLargeBufferSize>;
    using BufferVector = std::vector<std::unique_ptr<LargeBuffer>>;
    // BufferVector::value_type 是 std::vector<std::unique_ptr<FixedBuffer>> 的元素类型，也就是 std::unique_ptr<FixedBuffer>。
    using BufferPtr = BufferVector::value_type;
    void threadFunc();
    const int flushInterval_;       // 日志刷新时间
    std::atomic<bool> running_;
    const std::string basename_;
    const off_t rollSize_;      // 日志的最大滚动大小
    Thread thread_;             // 后端负责写日志的进程
    std::mutex mutex_;
    std::condition_variable cond_;
    
    BufferPtr currentBuffer_;   // 前端日志
    BufferPtr nextBuffer_;      // 预备缓冲区，切换时减少new操作
    BufferVector buffers_;      // 前端写满的会移动到这里，等待后端线程写入磁盘
};
