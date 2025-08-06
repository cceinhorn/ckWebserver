#pragma once

#include <map>
#include <string>
#include <cctype> 

#include "copyable.h"
#include "Timestamp.h"

using namespace std;

class HttpRequest : copyable
{
public:
    enum Method     // HTTP请求方法
    {
        kInvalid, kGet, kPost, kHead, kPut, kDelete
    };

    enum Version    // HTTP版本
    {
        kUnknown, kHttp10, kHttp11
    };

    HttpRequest()
    : method_(kInvalid),
      version_(kUnknown)
    {}

    void setVersion(Version v){ version_ = v; }

    Version getVersion() const { return version_ ;}

    bool setMethod(const char* start, const char* end){
        string m(start, end);
        
        if(m =="GET") method_ = kGet;
        else if (m =="POST") method_ = kPost;
        else if (m =="HEAD") method_ = kHead;
        else if (m =="PUT") method_ = kPut;
        else if (m =="DELETE") method_ = kDelete;       
        else method_ = kInvalid;
        return method_ != kInvalid;
    }

    Method getMethod() const { return method_; }

    const char* methodString() const
    {
        const char* result = "UNKNOWN";
        switch (method_)
        {
        case kGet:              // 获取资源，只读
            result = "GET";
            break;
        case kPost:             // 创建/提交数据，写  表单提交/注册登录/上传文件
            result = "POST";
            break;
        case kHead:             // 获取资源头部信息 只读 测试资源是否存在
            result = "HEAD";
            break;
        case kPut:              //  更新资源 写 更新文章，替代配置
            result = "PUT";
            break;
        case kDelete:           // 删除资源 写
            result = "DELETE";
            break;
        default:
            break;
        }
        return result;
    }

    void setPath(const char* start, const char* end){ path_.assign(start, end); }
    const string& path() const{ return path_; }

    void setQuery(const char* start, const char* end){ query_.assign(start, end); }
    const string& query() const{ return query_; }

    void setReceiveTime(Timestamp t){ receiveTime_ = t; }
    Timestamp receiveTime() const{ return receiveTime_; }

    // headers_ = {
    // { "Host", "www.example.com" },
    // { "Content-Type", "text/html" },
    // { "Connection", "keep-alive" }
    // };
    // 从一行 HTTP 头部字段中提取 key 和 value，并存入 headers_ 字典中
    void addHeader(const char* start, const char* colon, const char* end)
    {
        string field(start, colon);
        ++colon;

        while (colon < end && isspace(*colon))
        {
            ++colon;
        }

        string value(colon, end);

        while (!value.empty() && isspace(value[value.size()-1]))
        {
            value.resize(value.size()-1);
        }
        headers_[field] = value;
    }

    string getHeader(const string& field) const
    {
        string result;
        std::map<string, string>::const_iterator it = headers_.find(field);
        if (it != headers_.end())
        {
            result = it->second;
        }
        return result;
    }

    const std::map<string, string>& headers() const { return headers_; }

    void swap(HttpRequest& that)
    {
        std::swap(method_, that.method_);
        std::swap(version_, that.version_);
        path_.swap(that.path_);
        query_.swap(that.query_);
        headers_.swap(that.headers_);
    }

private:
    Method method_;
    Version version_;
    string path_;                           // 请求路径         /index.html
    string query_;                          // 查询字符串       ?id=123&name=abc
    Timestamp receiveTime_;                 // 请求接收时间戳    TcpServer 中记录
    std::map<string, string> headers_;      // 所有的请求字段       User-Agent, Host 等

};