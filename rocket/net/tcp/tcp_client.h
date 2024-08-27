#pragma once
#include "rocket/net/tcp/net_addr.h"
#include "rocket/net/eventloop.h"
#include "rocket/net/tcp/tcp_connection.h"
#include "rocket/net/tcp/abstract_protocol.h"
namespace rocket
{
    class TcpClient
    {
    public:
        TcpClient(NetAddr::s_ptr peer_addr);
        ~TcpClient();

        // 异步的进行connect
        // 如果connect成功，done会执行
        void connect(std::function<void()> done);

        // 异步的发送 Message
        // 如果发送 message成功，会调用done 函数，函数的入参就是message对象
        void writeMessage(AbstractProtocol::s_ptr message, std::function<void(AbstractProtocol::s_ptr)> done);

        // 异步的读取 Message
        // 如果读取 message成功，会调用done 函数，函数的入参就是message对象
        void readMessage(const std::string &req_id, std::function<void(AbstractProtocol::s_ptr)> done);

    private:
        NetAddr::s_ptr m_peer_addr;
        EventLoop *m_event_loop{NULL};

        int m_fd{-1};
        FdEvent *m_fd_event{NULL};
        TcpConection::s_ptr m_connection;
    };
}