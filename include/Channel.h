#pragma once

#include <memory>
#include <functional>

#include "noncopyable.h"
#include "Timestamp.h"

class EventLoop;

/**
    监测事件，client->fd->channel->eventloop->注册到epollpoller中
    fd事件发生
    epoll从红黑树上取下来放进链表中等待执行
    返回活跃channel
    |
    ^
    // 在对应的channel中取出事件回调函数
    eventloop轮询处理(handleevent)
**/
class Channel : noncopyable
{
public:
    // typedef std::function<void()> EventCallback;
    using EvenCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop *loop, int fd);
    ~Channel();

    // EventLoop中调用执行
    void handleEvent(Timestamp receiveTime);

    // 外部类通过以下函数设置回调函数用于handlevent执行回调
    void SetReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb); }
    void SetWriteCallback(EvenCallback cb) { writeCallback_ = std::move(cb); }
    void SetCloseCallback(EvenCallback cb) { closeCallback_ = std::move(cb); }
    void SetErrorCallback(EvenCallback cb) { errorCallback_ = std::move(cb); }

    // 防止channel被remove当channel还在执行回调时
    void tie(const std::shared_ptr<void> &);

    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_revents(int revents) { revents_ = revents; }

    // 按位epoll_ctl fd状态 add delete
    // enum EPOLL_EVENTS
    //   {
    //     EPOLLIN = 0x001,         0001
    // #define EPOLLIN EPOLLIN
    //     EPOLLPRI = 0x002,        0010
    // #define EPOLLPRI EPOLLPRI
    //     EPOLLOUT = 0x004,        1000
    // ...
    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    void disableAll() { events_ = kNoneEvent; update(); }

    bool isReading() const { return events_ & kReadEvent; }
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isNoneEvent() const { return events_ == kNoneEvent; }

    int index() { return index_; }
    void setindex(int index) { index_ = index; }

    // one loop per thread
    EventLoop *ownerLoop() { return loop_; }
    void remove();
private:
    void update();      // epoll_ctl
    void handleEventWithGuard(Timestamp receiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_;
    const int fd_;
    int events_;
    int revents_;
    int index_;         // channel/fd 当前是否在epoll中
    
    // 弱回调，改变TcpConnection的生命周期
    std::weak_ptr<void> tie_;   
    bool tied_;

    // 调用具体事件
    ReadEventCallback readCallback_;
    EvenCallback writeCallback_;
    EvenCallback closeCallback_;
    EvenCallback errorCallback_;
};