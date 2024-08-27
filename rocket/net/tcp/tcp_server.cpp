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
        m_io_thread_group->start(); // 所有IO线程启动loop循环
        m_main_event_loop->loop();  // 主线程启动loop循环
    }

    void TcpServer::init()
    {

        // 创建主acceptor
        m_acceptor = std::make_shared<TcpAcceptor>(m_local_addr); // 初始化服务器套接字操作(socket bind listen)

        // 主线程loop循环
        m_main_event_loop = EventLoop::GetCurrentEventLoop();

        // IO线程loop循环
        m_io_thread_group = new IOThreadGroup(2); // 开启2个IO线程

        // 服务器套接字(listenfd) 挂到红黑树上 epoll_wait进行监听
        m_listen_fd_event = new FdEvent(m_acceptor->getListenFd());
        // 绑定可读事件和连接处理器
        m_listen_fd_event->listen(FdEvent::IN_EVENT, std::bind(&TcpServer::onAccept, this));
        m_main_event_loop->addEpollEvent(m_listen_fd_event); // 监听事件挂到树上
    }

    // epoll_wait监听到listenfd可读 调用此事件处理器
    void TcpServer::onAccept()
    {
        auto re = m_acceptor->accept();       // 接收连接
        int client_fd = re.first;             // 已连接客户端fd
        NetAddr::s_ptr peer_addr = re.second; // 已连接客户端地址
        m_client_counts++;

        // TODO 把client添加到任意一个IO线程里面
        IOThread *io_thread = m_io_thread_group->getIOThread(); // 获取一个IO线程
        // 创建一个 已连接的connection
        TcpConection::s_ptr connection = std::make_shared<TcpConection>(io_thread->getEventLoop(), client_fd, 128, peer_addr);
        connection->setState(Conected);
        m_client.insert(connection);
        INFOLOG("TcpServer success get client, fd=%d", client_fd);
    }
}