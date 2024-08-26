#include "tcp_server.h"
#include "rocket/common/log.h"
#include "rocket/net/eventloop.h"
#include "rocket/net/tcp/tcp_connection.h"

namespace rocket
{
    TcpServer::TcpServer(NetAddr::s_ptr local_addr) : m_local_addr(local_addr)
    {

        init();
        INFOLOG("rocket TcpServer listen sucess on [%s]", m_local_addr->toString().c_str());
    }
    TcpServer::~TcpServer()
    {
        if (m_main_event_loop)
        {
            delete m_main_event_loop;
            m_main_event_loop = NULL;
        }
    }

    void TcpServer::start()
    {
        m_io_thread_group->start();
        m_main_event_loop->loop();
    }

    void TcpServer::init()
    {

        // 创建主acceptor
        m_acceptor = std::make_shared<TcpAcceptor>(m_local_addr);

        // 主线程loop循环
        m_main_event_loop = EventLoop::GetCurrentEventLoop();

        // IO线程loop循环
        m_io_thread_group = new IOThreadGroup(2);

        // listenfd事件监听添加进主event_loop循环中
        m_listen_fd_event = new FdEvent(m_acceptor->getListenFd());
        m_listen_fd_event->listen(FdEvent::IN_EVENT, std::bind(&TcpServer::onAccept, this));
        m_main_event_loop->addEpollEvent(m_listen_fd_event);
    }

    // 当发生连接事件
    void TcpServer::onAccept()
    {
        auto re = m_acceptor->accept();
        int client_fd = re.first;
        NetAddr::s_ptr peer_addr = re.second;
        // FdEvent client_fd_enevd(client_fd);
        m_client_counts++;

        // TODO 把client添加到任意一个IO线程里面
        IOThread *io_thread = m_io_thread_group->getIOThread();
        TcpConection::s_ptr connection = std::make_shared<TcpConection>(io_thread, client_fd, 128, peer_addr);
        connection->setState(Conected);
        m_client.insert(connection);
        INFOLOG("TcpServer success get client, fd=%d", client_fd);
    }
}