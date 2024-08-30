#pragma once
#include <memory>
#include "rocket/net/tcp/net_addr.h"
#include "rocket/net/eventloop.h"
#include "rocket/net/tcp/tcp_connection.h"
#include "rocket/net/coder/abstract_protocol.h"
namespace rocket
{
    class TcpClient
    {
    public:
        typedef std::shared_ptr<TcpClient> s_ptr;

        TcpClient(NetAddr::s_ptr peer_addr);
        ~TcpClient();

        // 异步的进行connect
        // 如果connect完成，done会执行
        void connect(std::function<void()> done);

        // 异步的发送 Message
        // 如果发送 message成功，会调用done 函数，函数的入参就是message对象
        void writeMessage(AbstractProtocol::s_ptr message, std::function<void(AbstractProtocol::s_ptr)> done);

        // 异步的读取 Message
        // 如果读取 message成功，会调用done 函数，函数的入参就是message对象
        void readMessage(const std::string &msg_id, std::function<void(AbstractProtocol::s_ptr)> done);

        // 结束loop循环
        void stop();

        int getConnectErrorCode();

        std::string getConnectErrorInfo();

        NetAddr::s_ptr getPeerAddr();
        NetAddr::s_ptr getlocalAddr();

        void initLocalAddr();

    private:
        NetAddr::s_ptr m_peer_addr;    // 服务器地址
        NetAddr::s_ptr m_local_addr;   // 客户端地址
        EventLoop *m_event_loop{NULL}; // 客户端eventlooop

        int m_fd{-1};                     // 客户端套接字
        FdEvent *m_fd_event{NULL};        // 客户端fd_event
        TcpConection::s_ptr m_connection; // 连接

        int m_connect_err_code{0};        // 连接错误码
        std::string m_connect_error_info; // 连接错误信息
    };
}