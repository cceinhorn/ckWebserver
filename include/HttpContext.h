#pragma once

#include "Timestamp.h"
#include "copyable.h"
#include "HttpRequest.h"


class Buffer;

class HttpContext : public copyable
{
public:
    enum HttpRequestParseState
    {
        kExpectRequestLine,
        kExpectHeaders,
        kExpectBody,
        kGotAll,
    };
    HttpContext()
        : state_(kExpectRequestLine)
    {
    }

    // 主解析
    bool parseRequest(Buffer* buf, Timestamp receiveTime);

    bool gotAll() const { return state_ == kGotAll; }

    // 重置状态，解析下一个请求
    void reset()    
    {
        state_ = kExpectRequestLine;
        HttpRequest dummy;
        request_.swap(dummy);
    }

    const HttpRequest& request() const { return request_; }
    HttpRequest& request() { return request_; }
    
private:
    // 解析请求行
    bool processRequestLine(const char* begin, const char* end);

    HttpRequestParseState state_;
    HttpRequest request_;
};