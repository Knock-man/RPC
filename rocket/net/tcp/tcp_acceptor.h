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
        typedef std::shared_ptr<TcpAcceptor> s_ptr;

        TcpAcceptor(NetAddr::s_ptr lcoal_addr);
        ~TcpAcceptor();

        int accept();

        int getListenFd();

    private:
        NetAddr::s_ptr m_local_addr; // 服务器监听的地址 addr->ip:port

        int m_family{-1};

        int m_listenfd{-1}; // 监听套接字
    };
}