#pragma once

#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>


class InetAddress
{
public:
    explicit InetAddress(uint16_t port = 0, std::string ip = "127.0.0.1");
    explicit InetAddress(const sockaddr_in &addr) : addr_(addr) { }

    std::string toIP() const;
    std::string toIpPort() const;
    uint16_t toPort() const;

    const sockaddr_in *getSockAddr() const { return &addr_; }
    void setSockAddr(const sockaddr_in &addr) { addr_ = addr; }

private:
    sockaddr_in addr_;

};

// struct sockaddr_in {
//     sa_family_t sin_family; // 地址族（AF_INET）
//     in_port_t sin_port;     // 端口（大端字节序）
//     struct in_addr sin_addr; // IP 地址（大端）
//     ...
// };