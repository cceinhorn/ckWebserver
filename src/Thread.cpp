#include <algorithm>
#include <cstdio>
#include <memory>
#include <semaphore.h>
#include <atomic>

#include "CurrentThread.h"
#include "Thread.h"


std::atomic_int Thread::numCreated_{0};  // 这行必须在Thread.cpp全局区域定义

Thread::Thread(ThreadFunc func, const std::string &name)
    : started_(false),
    joined_(false),
    tid_(0),
    func_(std::move(func)),
    name_(name)
{
    setDefaultName();
}


Thread::~Thread()
{
    if (started_ && !joined_) {
        thread_->detach();
    }
}


void Thread::start()
{
    started_ = true;
    sem_t sem;
    sem_init(&sem, false, 0);       // false指的是 不设置进程间共享
    thread_ = std::shared_ptr<std::thread>(new std::thread([&](){
        tid_ = CurrentThread::tid();
        sem_post(&sem);
        func_();        // 开启一个新线程 专门执行该线程函数
    }));

    sem_wait(&sem);  // 主线程阻塞等待，直到新线程执行 sem_post 唤醒
}

// join 主线程阻塞等待被调用线程去终止，主线程回收子线程  detach 主线程子线程脱离
void Thread::join()
{
    joined_ = true;
    thread_->join();
}


void Thread::setDefaultName()
{
    int num = numCreated_++;
    if (name_.empty()) {
        char buf[32] = {0};
        snprintf(buf, sizeof(buf),"Thread%d", num);
        name_ = buf;
    }
}