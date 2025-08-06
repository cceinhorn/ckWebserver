#include <errno.h>
#include <sys/uio.h>
#include <unistd.h>

#include "Buffer.h"

const char Buffer::kCRLF[] = "\r\n";


void Buffer::retrieve(size_t len)
{
    if (len < readableBytes())
    {
        readerIndex_ += len; 
    }
    else // len == readableBytes()
    {
        retrieveAll();
    }
}


void Buffer::retrieveAll()
{
    readerIndex_ = kCheapPrepend;
    writerIndex_ = kCheapPrepend;
}


// 把onMessage函数上报的Buffer数据 转成string类型的数据返回
std::string Buffer::retrieveAllAsString()
{ 
    return retrieveAsString(readableBytes()); 
}


std::string Buffer::retrieveAsString(size_t len)
{
    std::string result(peek(), len);
    retrieve(len); // 上面一句把缓冲区中可读的数据已经读取出来 这里肯定要对缓冲区进行复位操作
    return result;
}


void Buffer::ensureWritableBytes(size_t len)
{
    if (writableBytes() < len)
    {
        makeSpace(len); // 扩容
    }
}


void Buffer::makeSpace(size_t len)
{
    if (writableBytes() + prependableBytes() < len + kCheapPrepend)
    {
        buffer_.resize(writerIndex_ + len);
    }
    else
    {
        size_t readable = readableBytes();
        std::copy(begin() + readerIndex_,
                    begin() + writerIndex_,
                    begin() + kCheapPrepend);
        readerIndex_ = kCheapPrepend;
        writerIndex_ = readerIndex_ + readable;
    }
}


void Buffer::append(const char *data, size_t len)
{
    ensureWritableBytes(len);
    std::copy(data, data+len, beginWrite());
    writerIndex_ += len;
}


ssize_t Buffer::readFd(int fd, int *saveErrno)
{
    // extrabuf => buf
    char extrabuf[65536] = {0}; // 64KB

    /*
    struct iovec {
        ptr_t iov_base;     // start addreass
        size_t iov_len;     // space
    };
    */

    struct iovec vec[2];
    const size_t writable = writableBytes(); 

    // 第一块缓冲区，指向可写空间
    vec[0].iov_base = begin() + writerIndex_;
    vec[0].iov_len = writable;
    // 第二块缓冲区，指向栈空间
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);

    const int iovcnt = (writable < sizeof(extrabuf)) ? 2 : 1;       // 并不能预测fd数据大小再决定，经验先判断buffer是否足够大，决定是否使用额外空间
    const ssize_t n = ::readv(fd, vec, iovcnt);        // 分散读取，一次性读取多个缓冲区    优先vec[0]

    if (n < 0)
    {
        *saveErrno = errno;
    }
    else if (n <= writable)
    {
        writerIndex_ += n;
    }
    else // n-writable
    {
        writerIndex_ = buffer_.size();
        append(extrabuf, n - writable); 
    }
    return n;
}


ssize_t Buffer::writeFd(int fd, int *saveErrno)
{
    ssize_t n = ::write(fd, peek(), readableBytes());       // readerIndex_  ->    readableBytes  写入  fd
    if (n < 0)
    {
        *saveErrno = errno;
    }
    return n;
}