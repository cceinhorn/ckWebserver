#pragma once

#include "noncopyable.h"
#include "Channel.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"

class Acceptor : noncopyable
{
public:
    using NewConnectionCallback = std::function<void(int sockfd, const InetAddress &)>;

    Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback &cb) { NewConnectionCallback_ = cb; }
    bool listenning() const { return listenning_; }
    void listen();

private:
    void handleRead();

    NewConnectionCallback NewConnectionCallback_;
    EventLoop *loop_;       // mainloop
    bool listenning_;
    Socket acceptSocket_;
    Channel acceptChannel_;

};