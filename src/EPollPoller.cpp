#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>

#include "EPollPoller.h"
#include "Logger.h"
#include "Channel.h"

const int kNew = -1;        // channel 未添加
const int kAdded = 1;       // channel 已添加
const int kDeleted = 2;     // channel 已从poller删除
 
EPollPoller::EPollPoller(EventLoop *loop)
    : Poller(loop),
    epollfd_((::epoll_create1(EPOLL_CLOEXEC))),
    events_(kInitEventListSize)
{
    if (epollfd_ < 0) {
        // LOG_FATAL("epoll_creat error:%d \n", errno);
        LOG_FATAL << "epoll_create error: %d \n" << errno;
    }
}


EPollPoller::~EPollPoller()
{
    ::close(epollfd_);
}


Timestamp EPollPoller::poll(int timeoutmsg, ChannelList *activeChannels)
{   
    // timeoutmsg = 1000
    // LOG_INFO("func=%s => fd total count:%lu\n", __FUNCTION__, channels_.size());
    LOG_INFO << "fd total count: " << channels_.size();
    int numEvents = ::epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutmsg);
    int saveErrno = errno;
    Timestamp now(Timestamp::now());

    if (numEvents > 0)
    {
        // LOG_INFO("%d events happend\n", numEvents);
        LOG_INFO << "events happend: " << numEvents; // LOG_DEBUG最合理
        fillActiveChannel(numEvents, activeChannels);
        if (numEvents == events_.size())
        {
            events_.resize(events_.size() * 2);
        }
    }
    else if (numEvents == 0) 
    {
        // LOG_DEBUG("%s timeout!\n", __FUNCTION__);
        LOG_DEBUG << "timeout!";
    }
    else 
    {
        if (saveErrno != EINTR)
        {

        }
    }
    return now;
}


void EPollPoller::updateChannel(Channel *channel)
{
    const int index = channel->index();
    // LOG_INFO("func=%s => fd=%d events=%d index=%d\n", __FUNCTION__, channel->fd(), channel->events(), index);
    LOG_INFO << "func =>" << " fd: " << channel->fd() << " events=" << channel->events() << " index=" << index;
    
    if (index == kNew || index == kDeleted) {
        if (index == kNew) {
            int fd = channel->fd();
            channels_[fd] = channel;
        }
        else {

        }
        channel->setindex(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else {
        int fd = channel->fd();
        if (channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL, channel);
            channel->setindex(kDeleted);
        }
        else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}


void EPollPoller::removeChannel(Channel *channel)
{
    int fd = channel->fd();
    channels_.erase(fd);

    // LOG_INFO("func=%s => fd=%d\n", __FUNCTION__, fd);
    LOG_INFO << "removeChannel fd=" << fd;
    int index = channel->index();
    if (index == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setindex(kNew);
}


void EPollPoller::fillActiveChannel(int numEvents, ChannelList *activeChannels) const
{
    for (int i = 0; i < numEvents; i++) {
        Channel *channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);
    }
}


void EPollPoller::update(int operation, Channel *channel)
{
    epoll_event event;
    ::memset(&event, 0, sizeof(event));

    int fd = channel->fd();

    event.events = channel->events();
    event.data.fd = fd;
    event.data.ptr = channel;

    //                                           fd         感兴趣事件
    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0) {
        if (operation == EPOLL_CTL_DEL) {
            // LOG_ERROR("epoll_ctl del error:%d\n", errno);
            LOG_ERROR << "epoll_ctl del error: " << errno;
        }
        else {
            // LOG_FATAL("epoll_ctl add/mod error:%d\n", errno);
            LOG_FATAL << "epoll_ctl add/mod error: " << errno;
        }
    }
}