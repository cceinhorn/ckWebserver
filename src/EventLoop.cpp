#include <cstdint>
#include <mutex>
#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <memory>

#include "EventLoop.h"
#include "Poller.h"
#include "Channel.h"
#include "Logger.h"

__thread EventLoop *t_loopInThisThread = nullptr;

const int kPollTimems = 10000;      //ms


int createEventfd()
{
    int eventfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (eventfd < 0) {
        // LOG_FATAL("eventfd error:%d\n", errno);
        LOG_FATAL << "eventfd error: %d" << errno;
    }
    return eventfd;
}


EventLoop::EventLoop()
    : looping_(false),
    quit_(false),
    callingPendingFunctors_(false),
    threadId_(CurrentThread::tid()),
    poller_(Poller::newDefaultPoller(this)),
    wakeupFd_(createEventfd()),
    wakeupChannel_(new Channel(this, wakeupFd_) )
{
    // LOG_DEBUG("EventLoop created %p in thread %d\n", this, threadId_);
    LOG_DEBUG << "EventLoop created " << this << " in thread: " << threadId_;
    if (t_loopInThisThread) {
        // LOG_FATAL("Another EventLoop %p exists in this thread %d\n", t_loopInThisThread, threadId_);
        LOG_FATAL << "Another EventLoop " << t_loopInThisThread << " exists in this thread!" << threadId_;
    } 
    else {
        t_loopInThisThread = this;
    }

    wakeupChannel_->SetReadCallback(std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading();
}


EventLoop::~EventLoop()
{
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = nullptr;
}


void EventLoop::loop()
{
    looping_ = true; 
    quit_ = false;

    // LOG_INFO("EventLoop %p start looping\n", this);
    LOG_INFO << "EventLoop start looping!";

    while (!quit_) {
        activeChannels_.clear();        // 清空上轮
        pollRetureTime_ = poller_->poll(kPollTimems, &activeChannels_);     // epoll_wait()  这轮
        for (Channel *channel : activeChannels_) {
            channel->handleEvent(pollRetureTime_);
        }
        /**
         accept接收连接 => 将accept返回的connfd打包为Channel => TcpServer::newConnection通过轮询将TcpConnection对象分配给subloop处理
         mainloop调用queueInLoop将回调加入subloop（该回调需要subloop执行 但subloop还在poller_->poll处阻塞） queueInLoop通过wakeup将subloop唤醒
         **/
        doPendingFunctors();        // 上层回调 accept()时
    }
    // LOG_INFO("EventLoop %p stop looping.\n", this);
    LOG_INFO << "EventLoop stop looping!";
    looping_ = false;
}


void EventLoop::quit()
{
    quit_ = true;

    if (!isInLoopThread()) {    // subloop 的 quit() 可能不在它自己的线程里调用     主线程控制 subloop 停止   Timer 回调里调用 quit()
        wakeup();               // 需要quit的线程可能在epoll_wait中阻塞，需要唤醒，下一轮quit
    }
}


void EventLoop::runInLoop(Functor cb)
{
    if (isInLoopThread()) {
        cb();
    }
    else {
        queueInLoop(cb);        // 上层回调不在当前的loop中时，需要唤醒
    }
}

// 将cb放入队列，唤醒对应的loop   下一轮执行回调
void EventLoop::queueInLoop(Functor cb)
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        pendingFuntors_.emplace_back(cb);
    }
    if (!isInLoopThread() || callingPendingFunctors_) {         
        wakeup();
    }
}


void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = read(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        // LOG_ERROR("EventLoop::handleRead() reads %lu bytes instead of 8\n", n);
        LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8!";
    }
}


void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = write(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        // LOG_ERROR("EventLoop::wakeup() writes %lu bytes instead of 8\n", n);
        LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8!";
    }
}


void EventLoop::updateChannel(Channel *channel)
{
    poller_->updateChannel(channel);
}


void EventLoop::removeChannel(Channel *channel)
{
    poller_->removeChannel(channel);
}


bool EventLoop::hasChannel(Channel *channel)
{
    return poller_->hasChannel(channel);
}

 
void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        functors.swap(pendingFuntors_);             // 交换减少锁的临界区范围 提升效率  避免死锁
    }

    for (const Functor &functor :functors) {
        functor();      // 执行当前loop的回调
    }

    callingPendingFunctors_ = false;
}


