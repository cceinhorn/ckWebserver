#include <memory>
#include <functional>
#include <string.h>

#include "TcpServer.h"
#include "Logger.h"
#include "TcpConnection.h"


static EventLoop *CheckLoopNotNull(EventLoop *loop)
{
    if (loop == nullptr) {
        // LOG_FATAL("main Loop is NULL!");
        LOG_FATAL << "main Loop is NULL!";
    }
    return loop;
}


TcpServer::TcpServer(EventLoop *loop,
                     const InetAddress &listenAddr,
                     const std::string &nameArg,
                     Option option)
    : loop_(CheckLoopNotNull(loop))
    , ipPort_(listenAddr.toIpPort())
    , name_(nameArg)
    , acceptor_(new Acceptor(loop, listenAddr, option == kReusePort))
    , threadPool_(new EventLoopThreadPool(loop, name_))
    , connectionCallback_()
    , messageCallback_()
    , nextConnId_(1)
    , started_(0)
{
    // 当有新用户连接时，Acceptor类中绑定的acceptChannel_会有读事件发生，执行handleRead()调用TcpServer::newConnection回调
    acceptor_->setNewConnectionCallback(
        std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}


TcpServer::~TcpServer()
{
    for(auto &item : connections_) {
        TcpConnectionPtr conn(item.second);
        item.second.reset();    // 把原始的智能指针复位 让栈空间的TcpConnectionPtr conn指向该对象 当conn出了其作用域 即可释放智能指针指向的对象
        // 销毁连接
        conn->getLoop()->runInLoop(
            std::bind(&TcpConnection::connectDestroyed, conn));
    }
}


void TcpServer::setThreadNum(int numThreads)
{
    int numThreads_=numThreads;
    threadPool_->setThreadNum(numThreads_);
}

// 开启服务器监听
void TcpServer::start()
{
    if (started_.fetch_add(1) == 0) {           // 先返回再加1，防止多次启动
        threadPool_->start(threadInitCallback_);   
        loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));        // baseloop
    }
}


void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr)
{
    EventLoop *ioLoop = threadPool_->getNextLoop();
    char buf[64] = {0};
    snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
    ++nextConnId_;  
    std::string connName = name_ + buf;

    // LOG_INFO("TcpServer::newConnection [%s]- new connection [%s] from %s\n", 
    //       name_.c_str(), connName.c_str(), peerAddr.toIpPort().c_str());
    LOG_INFO << "TcpServer::newConnection [" << name_.c_str() << "]- new connection [" << connName.c_str() << "] from " << peerAddr.toIpPort().c_str();
    
    sockaddr_in local;
    ::memset(&local, 0, sizeof(local));
    socklen_t addrlen = sizeof(local);
    if(::getsockname(sockfd, (sockaddr *)&local, &addrlen) < 0)
    {
        // LOG_ERROR("sockets::getLocalAddr");
        LOG_ERROR << "sockets::getLocalAddr!";
    }

    InetAddress localAddr(local);
    TcpConnectionPtr conn(new TcpConnection(ioLoop,
                                            connName,
                                            sockfd,
                                            localAddr,
                                            peerAddr));
    connections_[connName] = conn;

    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);

    conn->setCloseCallback(
        std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));

    ioLoop->runInLoop(
        std::bind(&TcpConnection::connectEstablished, conn));
}


void TcpServer::removeConnection(const TcpConnectionPtr &conn)
{
    loop_->runInLoop(
        std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}


void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn)
{
    // LOG_INFO("TcpServer::removeConnectionInLoop [%s] - connection %s",
    //         name_.c_str(),conn->name().c_str());
    LOG_INFO << "TcpServer::removeConnectionInLoop [" << name_.c_str() << "] - connection %s"<< conn->name().c_str();

    connections_.erase(conn->name());
    EventLoop *ioLoop = conn->getLoop();
    ioLoop->queueInLoop(
        std::bind(&TcpConnection::connectDestroyed, conn));
}