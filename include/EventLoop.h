#pragma once

#include <functional>
#include <vector>
#include <atomic>
#include <memory>
#include <mutex>

#include "noncopyable.h"
#include "Timestamp.h"
#include "CurrentThread.h"
#include "Channel.h"
#include "Poller.h"

class EventLoop : noncopyable
{
public: 
    using Functor = std::function<void()>;

    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    Timestamp pollReturnTime() const { return pollRetureTime_; }

    void runInLoop(Functor cb);         // 在当前loop中执行
    void queueInLoop(Functor cb);       // 把上册注册的回调函数放入队列     唤醒loop所在线程执行cb
    void wakeup();                      // 通过eventfd唤醒loop所在的线程

    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);
    bool hasChannel(Channel *channel);

    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

private:
    void handleRead();          // 给eventfd返回的文件描述符wakefd绑定的事件回调   当wakeup()时  即有事件发生
    void doPendingFunctors();   // 执行上层回调

    using ChannelList = std::vector<Channel *>;
    
    std::atomic_bool looping_;  // 主线程或其他线程控制本loop启停，与本线程存在竞态条件
    std::atomic_bool quit_;         

    const pid_t threadId_;          // 记录当前eventloop是被哪个线程id创建的

    Timestamp pollRetureTime_;      // Poller返回发生事件的channels的时间点
    std::unique_ptr<Poller> poller_;

    int wakeupFd_;                  // 本loop的文件描述符，用于唤醒
    std::unique_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;    // 

    std::atomic_bool callingPendingFunctors_;       // 标识当前loop是否需要执行的回调操作
    std::vector<Functor> pendingFuntors_;           // 存储loop需要执行的所有回调操作
    std::mutex mutex_;                              // 互斥锁 保护vector容器的线程安全
};  
