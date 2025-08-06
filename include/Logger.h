#pragma once

#include <string>
#include <stdio.h>
#include <string.h>
#include <string>
#include <errno.h>
#include<functional>

#include "noncopyable.h"
#include "Timestamp.h"
#include "LogStream.h"

#define OPEN_LOGGING

// LOG_INFO("%s %d", arg1, arg2)
// #define LOG_INFO(...) \
//     do \
//     { \
//         Logger &logger = Logger::instance(); \
//         logger.setLoglevel(INFO); \
//         char buf[1024] = {0}; \
//         snprintf(buf, 1024, __VA_ARGS__); \
//         logger.log(buf); \
//     } while (0)                                                   


// #define LOG_ERROR(...) \
//     do \
//     { \
//         Logger &logger = Logger::instance(); \
//         logger.setLoglevel(ERROR); \
//         char buf[1024] = {0}; \
//         snprintf(buf, 1024, __VA_ARGS__); \
//         logger.log(buf); \
//     } while (0) 


// #define LOG_FATAL(...) \
//     do \
//     { \
//         Logger &logger = Logger::instance(); \
//         logger.setLoglevel(FATAL); \
//         char buf[1024] = {0}; \
//         snprintf(buf, 1024, __VA_ARGS__); \
//         logger.log(buf); \
//         exit(-1); \
//     } while (0) 

// #ifdef MUDEBUG
// #define LOG_DEBUG(...) \
//     do \
//     { \
//         Logger &logger = Logger::instance(); \
//         logger.setLoglevel(DEBUG); \
//         char buf[1024] = {0}; \
//         snprintf(buf, 1024, __VA_ARGS__); \
//         logger.log(buf); \
//     } while (0) 
// #else
// #define LOG_DEBUG(...)
// #endif

// SourceFile的作用是提取文件名
class SourceFile
{
public:
    explicit SourceFile(const char* filename)
        : data_(filename)
    {
        /**
         * 找出data中出现/最后一次的位置，从而获取具体的文件名
         * 2022/10/26/test.log
         */
        const char* slash = strrchr(filename, '/');
        if (slash)
        {
            data_ = slash + 1;
        }
        size_ = static_cast<int>(strlen(data_));
    }

    const char* data_;
    int size_;
};


// enum LogLevel 
// {
//     INFO,   // 普通信息
//     ERROR,  // 错误信息   
//     FATAL,  // core dump信息
//     DEBUG,  // 调试信息
// };

// muduo为单例模式，先改为双缓冲异步日志
// class Logger : noncopyable  // 私有继承
// {
// public:
//     // 获取日志唯一的实例对象，单例模式 noncopyable中构造函数为私有
//     static Logger &instance();
//     // 设置日志等级
//     void setLoglevel(int level);
//     // 写日志
//     void log(std::string msg);
// private:
//     int loglevel_;
// };
class Logger
{
public:
   enum LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        LEVEL_COUNT,
    };
    Logger(const char *filename, int line, LogLevel level);
    ~Logger();
    // 流是会改变的
    LogStream& stream() { return impl_.stream_; }


    // 输出函数和刷新缓冲区函数
    using OutputFunc = std::function<void(const char* msg, int len)>;
    using FlushFunc = std::function<void()>;
    static void setOutput(OutputFunc);
    static void setFlush(FlushFunc);

private:
    class Impl
    {
    public:
        using LogLevel=Logger::LogLevel;
        Impl(LogLevel level,int savedErrno,const char *filename, int line);
        void formatTime();
        void finish(); // 添加一条log消息的后缀

        Timestamp time_;
        LogStream stream_;
        LogLevel level_;
        int line_;
        SourceFile basename_;
    };

private:
    Impl impl_;
};


// 获取errno信息
const char* getErrnoMsg(int savedErrno);
/**
 * 当日志等级小于对应等级才会输出
 * 比如设置等级为FATAL，则logLevel等级大于DEBUG和INFO，DEBUG和INFO等级的日志就不会输出
 */
#ifdef OPEN_LOGGING
#define LOG_DEBUG Logger(__FILE__, __LINE__, Logger::DEBUG).stream()
#define LOG_INFO Logger(__FILE__, __LINE__, Logger::INFO).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::FATAL).stream()
#else
#define LOG(level) LogStream()
#endif