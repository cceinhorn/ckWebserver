#pragma once

#include <csignal>
#include <memory>
#include <atomic>
#include <boost/any.hpp>

#include "InetAddress.h"
#include "Timestamp.h"
#include "noncopyable.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Callback.h"
#include "Buffer.h"


// Tcpsever Acceptor 新链接通过accept得到connfd   Tcpconnection设置回调  channel poller

class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop *loop,
                    const std::string &nameArg,
                    int sockfd,
                    const InetAddress &localAddr,
                    const InetAddress &peerAddr);
    ~TcpConnection();

    EventLoop *getLoop() const { return loop_; }
    const std::string &name() const { return name_; }
    const InetAddress &localAddress() const { return localAddr_; }
    const InetAddress &peerAddress() const { return peerAddr_; }

    bool connected() const { return state_ == kConnected; }
    bool disconnected() const { return state_ == kDisconnected; }

    void send(const std::string &buf);
    void shutdown();    // 关闭写端
    
    // pendingfunctors上层回调
    void setConnectionCallback(const ConnectionCallback &cb) { connectioncallback_ = cb; }
    void setMessageCallback(const MessageCallback &cb) { messagecallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb) { writeCompleteCallback_ = cb; }
    void setCloseCallback(const CloseCallback &cb) { closeCallback_ = cb; } 
    void setHighWaterMarkCallback(const HighWaterMarkCallback &cb, size_t highWaterMark) { highWaterMarkCallback_ = cb; highWaterMark_ = highWaterMark; }

    void connectEstablished();
    void connectDestroyed();

    void setContext(const boost::any& context){ context_ = context; }
    const boost::any& getContext() const { return context_; }
    boost::any* getMutableContext() {return &context_; }

private:
    enum State
    {
        kDisconnected,
        kconnecting,
        kConnected,
        kDisconnecting
    };
    void setState(State state) { state_ = state;}

    void handleRead(Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();

    void sendInLoop(const void *data, size_t len);
    void shutdownInLoop();

    EventLoop *loop_;
    std::string name_;
    const InetAddress localAddr_;
    const InetAddress peerAddr_;
    std::atomic<int> state_;
    bool reading_;

    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;

    ConnectionCallback connectioncallback_;
    MessageCallback messagecallback_;
    WriteCompleteCallback writeCompleteCallback_;
    CloseCallback closeCallback_;
    HighWaterMarkCallback highWaterMarkCallback_;       // 收发速度
    size_t  highWaterMark_;

    Buffer inputBuffer_;
    Buffer outputBuffer_;

    boost::any context_;
};
