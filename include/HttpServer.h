#pragma once

#include <functional>

#include "TcpServer.h"
#include "noncopyable.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Timestamp.h"


class HttpServer : noncopyable
{
public:
    using HttpCallback = std::function<void(const HttpRequest&, HttpResponse*)>;

    HttpServer(EventLoop *loop,
            const InetAddress &addr, 
            const std::string &name);
        
    void setHttpCallback(const HttpCallback& cb)
    {
        httpCallback_ = cb;
    }

    void setThreadNum(int numThreads)
    {
        server_.setThreadNum(numThreads);
    }

    void start();

private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn,
                 Buffer* buf,
                 Timestamp receiveTime);   

    void onRequest(const TcpConnectionPtr&, const HttpRequest&);
    
    EventLoop *loop_;           // mainloop
    TcpServer server_;
    HttpCallback httpCallback_;
};