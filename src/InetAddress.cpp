#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>

#include "InetAddress.h"

// 网络字节序  大端   主机字节序   小端
// 在 小端系统（如 x86）：输出 78 56 34 12

// 在 大端系统（如某些网络设备或 ARM 可配置）：输出 12 34 56 78
InetAddress::InetAddress(uint16_t port, std::string ip)
{
    ::memset(&addr_, 0, sizeof(addr_));         // 清零     另外 void bzero()
    addr_.sin_family = AF_INET;
    addr_.sin_port = ::htons(port);         // 本地字节序转为网络字节序
    addr_.sin_addr.s_addr = ::inet_addr(ip.c_str());
}

// struct sockaddr_in {
//     sa_family_t    sin_family; /* address family: AF_INET */
//     in_port_t      sin_port;   /* port in network byte order */
//     struct in_addr sin_addr;   /* internet address */
// };

// /* Internet address. */
// struct in_addr {
//     uint32_t       s_addr;     /* address in network byte order */
// };

std::string InetAddress::toIP() const
{
    char buf[64] = {0};
    ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
    return buf;
}


std::string InetAddress::toIpPort() const
{
    char buf[64] = {0};
    ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
    size_t end = ::strlen(buf);
    uint16_t port = ::ntohs(addr_.sin_port);
    sprintf((buf + end), ":%u", port);
    return buf;
}


uint16_t InetAddress::toPort() const
{
    return ::ntohs(addr_.sin_port);
}
