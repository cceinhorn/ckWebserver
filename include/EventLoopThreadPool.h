#pragma once

#include <memory>
#include <vector>

#include "noncopyable.h"
#include "EventLoop.h"
#include "EventLoopThread.h"

class EventLoopThreadPool : noncopyable
{
public:
    using ThreadInitCallback = std::function<void(EventLoop *)>;

    EventLoopThreadPool(EventLoop *baseloop, const std::string &nameArg);
    ~EventLoopThreadPool();

    void setThreadNum(int numThreads) { numThreads_ = numThreads; }
    void start(const ThreadInitCallback &cb = ThreadInitCallback());

    EventLoop *getNextLoop();       // 轮询分配channel

    std::vector<EventLoop *> getAllLoops();

    bool started() const { return started_; }
    const std::string name() const {return name_; }

private:
    EventLoop *baseLoop_;       // 为1表示为mainloop  否则创建多个subloop
    bool started_;
    std::string name_;
    int numThreads_;
    int next_;      // 轮询下标
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop *> loops_;

};