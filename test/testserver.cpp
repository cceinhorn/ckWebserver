#include <functional>
#include <string>

#include "TcpServer.h"
#include "Logger.h"


class EchoServer
{
public:
    EchoServer(EventLoop *loop, const InetAddress &addr, const std::string &name)
        : server_(loop, addr, name)
        , loop_(loop)
    {
        server_.setConnectionCallback(std::bind(&EchoServer::onConnection, this, std::placeholders::_1));

        server_.setMessageCallback(std::bind(&EchoServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        
        server_.setThreadNum(5);
    }

    void start()
    {
        server_.start();
    }

private:
    void onConnection(const TcpConnectionPtr &conn)
    {
        if (conn->connected()) {
            // LOG_INFO("Connection UP : %s", conn->peerAddress().toIpPort().c_str());
            LOG_INFO << "Connection UP : " << conn->peerAddress().toIpPort().c_str();
        }
        else {
            // LOG_INFO("Connection Down : %s", conn->peerAddress().toIpPort().c_str());
            LOG_INFO << "Connection Down : %s" << conn->peerAddress().toIpPort().c_str();
        }
    }

    void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time)
    {
        std::string msg = buf->retrieveAllAsString();
        conn->send(msg);
        conn->shutdown();
    }

    EventLoop *loop_;
    TcpServer server_;
};


int main()
{
    EventLoop loop;     // baseloop
    InetAddress addr(8002, "127.0.0.1");
    EchoServer server(&loop, addr, "EchoServer");
    server.start();
    loop.loop();
    return 0;
}