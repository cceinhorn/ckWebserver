#pragma once

#include <functional>
#include <atomic>
#include <memory>
#include <string>
#include <sys/types.h>
#include <thread>

#include "noncopyable.h"

class Thread : noncopyable
{
public:
    using ThreadFunc = std::function<void()>;

    explicit Thread(ThreadFunc, const std::string &name = std::string());
    ~Thread();

    void start();
    void join();

    bool started() { return started_; }
    pid_t tid() const {return tid_; }
    const std::string &name() const { return name_; }
    static int numCreated() { return numCreated_; }

private:
    void setDefaultName();

    ThreadFunc func_;
    bool started_;
    bool joined_;
    std::shared_ptr<std::thread> thread_;
    pid_t tid_;
    std::string name_;
    static std::atomic_int numCreated_;
};