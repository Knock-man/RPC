#pragma once
#include <memory>
#include <assert.h>

#include "../../rocket/common/log.h"
#include "net_addr.h"
namespace rocket
{
    class TcpAcceptor
    {
    public:
        TcpAcceptor(NetAddr::s_ptr lcoal_addr);
        ~TcpAcceptor();

        int accept();

    private:
        NetAddr::s_ptr m_local_addr; // 服务器监听的地址 addr->ip:port

        int m_family{-1};

        int m_listenfd{-1}; // 监听套接字
    };
}