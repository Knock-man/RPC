#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "tcp_acceptor.h"

namespace rocket
{
    // socket()  bind()  listen()
    TcpAcceptor::TcpAcceptor(NetAddr::s_ptr lcoal_addr) : m_local_addr(lcoal_addr)
    {
        // 检查地址有效性
        if (!lcoal_addr->checkValid())
        {
            ERRORLOG("invalid local addr %s", lcoal_addr->toString().c_str());
            exit(0);
        }

        // 网络通信 socket()  bind()  listen()  端口复用
        m_family = m_local_addr->getFamily();
        m_listenfd = socket(m_family, SOCK_STREAM, 0);
        if (m_listenfd < 0)
        {
            ERRORLOG("invalid listenfd %d", m_listenfd);
            exit(0);
        }

        // 设置端口复用
        int val = 1;
        if (setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) != 0)
        {
            ERRORLOG("setsockopt REUSEADDR error, errno=%d,errno=%s", errno, strerror(errno));
        }

        // bind()
        socklen_t len = m_local_addr->getSockLen();
        if (bind(m_listenfd, m_local_addr->getSockAddr(), len) != 0)
        {
            ERRORLOG("bind() error, errno=%d,error=%s", errno, strerror(errno));
            exit(0);
        }

        // listen()
        if (listen(m_listenfd, 1000) != 0)
        {
            ERRORLOG("listen() error, errno=%d,error=%s", errno, strerror(errno));
            exit(0);
        }
    }
    TcpAcceptor::~TcpAcceptor()
    {
    }

    // 连接客户端
    // 返回<客户端fd，客户端地址>
    std::pair<int, NetAddr::s_ptr> TcpAcceptor::accept()
    {
        if (m_family == AF_INET)
        {
            sockaddr_in client_addr; // 客户端地址
            memset(&client_addr, 0, sizeof(client_addr));
            socklen_t client_addr_len = sizeof(client_addr);
            int client_fd = ::accept(m_listenfd, reinterpret_cast<sockaddr *>(&client_addr), &client_addr_len);
            if (client_fd < 0)
            {
                ERRORLOG("accept() error, errno=%d,error=%s", errno, strerror(errno));
            }

            IPNetAddr::s_ptr peer_addr = std::make_shared<IPNetAddr>(client_addr);
            INFOLOG("A client have accepted success,peer addr [%s]", peer_addr->toString().c_str());
            return std::make_pair(client_fd, peer_addr);
        }
        else
        {
            //...
            return std::make_pair(-1, nullptr);
        }
    }

    int TcpAcceptor::getListenFd()
    {
        return m_listenfd;
    }
}