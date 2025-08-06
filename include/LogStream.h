#pragma once
#include <string.h>
#include <string>
#include <algorithm>

#include "noncopyable.h"
#include "FixedBuffer.h"


class GeneralTemplate : noncopyable
{
public:
    GeneralTemplate()
        : data_(nullptr),
          len_(0)
    {}

    explicit GeneralTemplate(const char* data, int len)
        : data_(data),
          len_(len)
    {}

    const char* data_;
    int len_;
};


// LogStream类用于管理日志输出流，重载输出流运算符<<，将各种类型的值写入内部缓冲区
class LogStream : noncopyable
{
public:
    // 定义一个Buffer类型，使用固定大小的缓冲区
    using Buffer = FixedBuffer<kSmallBufferSize>;

    // 将指定长度的字符数据追加到缓冲区
    void append(const char *buffer, int len)
    {
        buffer_.append(buffer, len);
    }

    // 返回当前缓冲区的常量引用
    const Buffer &buffer() const
    {
        return buffer_;
    }

    // 重置缓冲区，将当前指针重置到缓冲区的起始位置
    void reset_buffer()
    {
        buffer_.reset();
    }

    // 重载输出流运算符<<，用于将布尔值写入缓冲区
    LogStream &operator<<(bool express);

    // 重载输出流运算符<<，用于将短整型写入缓冲区
    LogStream &operator<<(short number);
    // 重载输出流运算符<<，用于将无符号短整型写入缓冲区
    LogStream &operator<<(unsigned short);
    // 重载输出流运算符<<，用于将整型写入缓冲区
    LogStream &operator<<(int);
    // 重载输出流运算符<<，用于将无符号整型写入缓冲区
    LogStream &operator<<(unsigned int);
    // 重载输出流运算符<<，用于将长整型写入缓冲区
    LogStream &operator<<(long);
    // 重载输出流运算符<<，用于将无符号长整型写入缓冲区
    LogStream &operator<<(unsigned long);
    // 重载输出流运算符<<，用于将长长整型写入缓冲区
    LogStream &operator<<(long long);
    // 重载输出流运算符<<，用于将无符号长长整型写入缓冲区
    LogStream &operator<<(unsigned long long);

    // 重载输出流运算符<<，用于将浮点数写入缓冲区
    LogStream &operator<<(float number);
    // 重载输出流运算符<<，用于将双精度浮点数写入缓冲区
    LogStream &operator<<(double);

    // 重载输出流运算符<<，用于将字符写入缓冲区
    LogStream &operator<<(char str);
    // 重载输出流运算符<<，用于将C风格字符串写入缓冲区
    LogStream &operator<<(const char *);
    // 重载输出流运算符<<，用于将无符号字符指针写入缓冲区
    LogStream &operator<<(const unsigned char *);
    // 重载输出流运算符<<，用于将std::string对象写入缓冲区
    LogStream &operator<<(const std::string &); 
    // (const char*, int)的重载
    LogStream& operator<<(const GeneralTemplate& g);
private:
    // 定义最大数字大小常量
    static constexpr int kMaxNumberSize = 32;

    // 对于整型需要特殊的处理，模板函数用于格式化整型
    template <typename T>
    void formatInteger(T num);

    Buffer buffer_;
};


// 模板函数定义放在头文件中，确保链接时实例化成功
template <typename T>
void LogStream::formatInteger(T num)
{
    if (buffer_.avail() >= kMaxNumberSize)
    {
        char *start = buffer_.current();
        char *cur = start;
        static const char digits[] = "9876543210123456789";
        static const char *zero = digits + 9;
        bool negative = (num < 0); // 判断num是否为负数
        do
        {
            int remainder = static_cast<int>(num % 10);
            (*cur++) = zero[remainder];
            num /= 10;
        } while (num != 0);
        if (negative)
        {
            *cur++ = '-';
        }
        *cur = '\0';
        std::reverse(start, cur);
        int length = static_cast<int>(cur - start);
        buffer_.add(length);
    }
}


