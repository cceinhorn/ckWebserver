#include <sys/socket.h>

#include "Acceptor.h"
#include "Logger.h"
#include "InetAddress.h"

static int createNonblocking()
{
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sockfd < 0) {
        // LOG_FATAL("%s:%s:%d listen socket create err:%d\n", __FILE__, __FUNCTION__, __LINE__, errno);
        LOG_FATAL << "listen socket create err! " << errno;
    }
    return sockfd;
}


Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport)
   : loop_(loop)
   , acceptSocket_(createNonblocking())
   , acceptChannel_(loop, acceptSocket_.fd())
   , listenning_(false)
{
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setReusePort(true);
    acceptSocket_.bindAddress(listenAddr);
    acceptChannel_.SetReadCallback(std::bind(&Acceptor::handleRead, this));
}


Acceptor::~Acceptor()
{
    acceptChannel_.disableAll();
    acceptChannel_.remove();
}


void Acceptor::listen()
{
    listenning_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading();     // 注册到Poller
}

// listen到新请求
void Acceptor::handleRead()
{
    InetAddress peerAddr;
    int connfd = acceptSocket_.accept(&peerAddr);
    if (connfd >= 0) {
        if (NewConnectionCallback_) {
            NewConnectionCallback_(connfd, peerAddr);
        }
        else {
            ::close(connfd);
        }
    }
    else {
        // LOG_ERROR("%s:%s:%d accept err:%d\n", __FILE__, __FUNCTION__, __LINE__, errno);
        LOG_ERROR << "accept Err! ";
        if (errno == EMFILE) {      // EMFILE 文件描述符到达上限    2e20
            // LOG_ERROR("%s:%s:%d sockfd reached limit\n", __FILE__, __FUNCTION__, __LINE__);
            LOG_ERROR << "sockfd reached limit! ";
            // 优雅地丢弃
            // ::close(idleFd_);
            // idleFd_ = ::accept(acceptSocket_.fd(), NULL, NULL);
            // ::close(idleFd_);
            // idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }
}