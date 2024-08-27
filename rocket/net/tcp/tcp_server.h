// 服务器
#pragma once
#include <set>
#include "tcp_acceptor.h"
#include "net_addr.h"
#include "../eventloop.h"
#include "../io_thread_group.h"
#include "rocket/net/tcp/tcp_connection.h"
namespace rocket
{
    class TcpServer
    {
    public:
        TcpServer(NetAddr::s_ptr local_addr);
        ~TcpServer();

        void start();

    private:
        void init();

        // 当有新客户端连接之后需要执行
        void onAccept();

    private:
        TcpAcceptor::s_ptr m_acceptor; // acceptor()  listen_fd

        NetAddr::s_ptr m_local_addr; // 本地服务器地址

        EventLoop *m_main_event_loop{NULL}; // mainReactor   主线程eventloop

        IOThreadGroup *m_io_thread_group{NULL}; // subReactor   IO线程组(每个线程一个eventloop)

        FdEvent *m_listen_fd_event; // 服务器套接字 FdEvent

        int m_client_counts{0}; // 连接的客户端数量

        std::set<TcpConection::s_ptr> m_client; // 连接的客户端集合
    };
}