#pragma once
#include "tcp_acceptor.h"
#include "net_addr.h"
#include "../eventloop.h"
#include "../io_thread_group.h"
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
        TcpAcceptor::s_ptr m_acceptor;

        NetAddr::s_ptr m_local_addr; // 本地监听地址

        EventLoop *m_main_event_loop{NULL}; // mainReactor

        IOThreadGroup *m_io_thread_group{NULL}; // subReactor

        FdEvent *m_listen_fd_event;

        int m_client_counts{0};
    };
}