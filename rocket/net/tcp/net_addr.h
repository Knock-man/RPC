// 地址封装
#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <memory>
#include <string>
namespace rocket
{
    // 网络地址抽象类
    class NetAddr
    {
    public:
        typedef std::shared_ptr<NetAddr> s_ptr;

        virtual sockaddr *getSockAddr() = 0; // 获取网络地址
        virtual socklen_t getSockLen() = 0;  // 获取地址长度
        virtual int getFamily() = 0;         // 获取地址族
        virtual std::string toString() = 0;  // 192.168.0.1:8888
        virtual bool checkValid() = 0;       // 地址是否有效
    };

    // IP4地址
    class IPNetAddr : public NetAddr
    {
    public:
        IPNetAddr(const std::string &ip, uint16_t port); // ip port

        IPNetAddr(const std::string &addr); // 127.0.0.1:8888

        IPNetAddr(sockaddr_in addr); // sockaddr_in

        sockaddr *getSockAddr(); // 获取 sockaddr*

        socklen_t getSockLen(); // 获取地址长度

        int getFamily(); // 获取地址族

        std::string toString(); // 获取 127.0.0.1:7878

        bool checkValid(); // 地址是否有效

    private:
        std::string m_ip;   // IP
        uint16_t m_port{0}; // PORT
        sockaddr_in m_addr; // 地址
    };
}