#pragma once

#include <vector>
#include <sys/epoll.h>

#include "Channel.h"
#include "Poller.h"
#include "Timestamp.h"

// 1、epoll_createstd
// 2、epoll_ctl (add/mod/del)
// 3、epoll_wait


class EPollPoller : public Poller
{
public:
    EPollPoller(EventLoop *loop);
    ~EPollPoller() override;

    // override virtual
    Timestamp poll(int timeoutmsg, ChannelList *activeChannels) override;
    void updateChannel(Channel *channel) override;
    void removeChannel(Channel *channel) override;
 
private:
    static const int kInitEventListSize = 16;
    
    void fillActiveChannel(int numEvents, ChannelList *activeChannels) const;

    // 调用 epoll_ctl
    void update(int operation, Channel *channel);   

    using EventList = std::vector<epoll_event>;

    // epoll_creat创建返回的fd保存在epollfd_中
    int epollfd_;
    // 存放返回epoll_wait返回的所有事件的文件描述符事件集合
    EventList events_;      
};

// typedef union epoll_data {
//     void    *ptr;
//     int      fd;
//     uint32_t u32;
//     uint64_t u64;
// } epoll_data_t;

// struct epoll_event {
//     uint32_t     events;    /* Epoll events */
//     epoll_data_t data;      /* User data variable */
// };