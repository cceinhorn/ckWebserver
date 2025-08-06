#include <memory>
#include <sys/epoll.h>

#include "Channel.h"
#include "Logger.h"
#include "EventLoop.h"

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

// enum EPOLL_EVENTS
//   {
//     EPOLLIN = 0x001,         0000 0000 0001        输入read
// #define EPOLLIN EPOLLIN
//     EPOLLPRI = 0x002,        0000 0000 0010        紧急事件
// #define EPOLLPRI EPOLLPRI        
//     EPOLLOUT = 0x004,        0000 0000 0100        输出write
// #define EPOLLOUT EPOLLOUT
//     EPOLLERR = 0x008,        0000 0000 1000        错误事件
// #define EPOLLERR EPOLLERR
//     EPOLLHUP = 0x010,        0000 0001 0000        连接挂起 
// #define EPOLLHUP EPOLLHUP
// ...

Channel::Channel(EventLoop *loop, int fd) 
    : loop_(loop), 
    fd_(fd),
    events_(0),
    revents_(0), 
    index_(-1),
    tied_(false) {}


Channel::~Channel() {}


void Channel::tie(const std::shared_ptr<void> &obj)
{
    tie_ = obj;
    tied_ = true;
}


void Channel::update()
{
    loop_->updateChannel(this);
}

void Channel::remove()
{
    loop_->removeChannel(this);
}


void Channel::handleEvent(Timestamp receiveTime) 
{
    if (tied_) {
        std::shared_ptr<void> guard = tie_.lock();
        if (guard) {
            handleEventWithGuard(receiveTime);
        }
    }
    else {
        handleEventWithGuard(receiveTime);
    }
}


void Channel::handleEventWithGuard(Timestamp receiveTime)
{
    // LOG_INFO("channel handleEvent revents:%d\n", revents_);
    LOG_INFO << "Channel handleEvent revents: " << revents_;
    // 关闭     识别EPOLLIN是半关闭 还是可以从socket中read剩下数据，并不是直接断掉
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {   // 当TcpConnection对应的channel通过shutdown关闭时，epoll触发EPOLLHUP
        if (closeCallback_) {
            closeCallback_();
        }
    }
    // 错误
    if (revents_ & EPOLLERR)
    {
        if (errorCallback_) {
            errorCallback_();
        }
    }
    // 读
    if (revents_ & (EPOLLIN | EPOLLPRI)) {
        if (readCallback_) {
            readCallback_(receiveTime);
        }
    }
    // 写
    if (revents_ &  EPOLLOUT) {
        if (writeCallback_) {
            writeCallback_();
        }
    }
}






