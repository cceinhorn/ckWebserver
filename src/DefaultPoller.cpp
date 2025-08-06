#include "EPollPoller.h"
#include "Poller.h"

Poller *Poller::newDefaultPoller(EventLoop *loop)
{
    if (::getenv("MUDUO_USE_POLL")) {
        return nullptr;     // poll实例
    }
    else {
        return new EPollPoller(loop);   //  epoll实例
    }
}

