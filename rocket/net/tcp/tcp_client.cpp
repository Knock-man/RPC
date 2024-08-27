#include "rocket/net/tcp/tcp_client.h"
#include "rocket/common/log.h"
#include "rocket/net/fd_event_group.h"
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
namespace rocket
{
    TcpClient::TcpClient(NetAddr::s_ptr peer_addr) : m_peer_addr(peer_addr)
    {
        m_event_loop = EventLoop::GetCurrentEventLoop();       // 获取当前线程eventloop
        m_fd = socket(peer_addr->getFamily(), SOCK_STREAM, 0); // 创建fd
        if (m_fd < 0)
        {
            ERRORLOG("TcpClient::TcpClient() error, faild to create fd");
            return;
        }

        // 创建fd_event
        m_fd_event = FdEventGroup::getFdEventGroup()->getFdEvent(m_fd);
        m_fd_event->setNonBlock();

        // 创建connection
        m_connection = std::make_shared<TcpConection>(m_event_loop, m_fd, 128, peer_addr, TcpConectionByClient);
        m_connection->setConectionType(TcpConectionByClient);
    }
    TcpClient::~TcpClient()
    {
        if (m_fd > 0)
        {
            close(m_fd);
        }
    }

    // 异步的进行connect
    // 如果connect成功，done会执行
    void TcpClient::connect(std::function<void()> done)
    {
        int rt = ::connect(m_fd, m_peer_addr->getSockAddr(), m_peer_addr->getSockLen());
        if (rt == 0)
        {
            DEBUGLOG("connect success");
            m_connection->setState(Conected);
            if (done)
                done();
        }
        else if (rt == -1)
        {
            if (errno == EINPROGRESS) // 异步非阻塞connect 连接请求已经开始，连接尚未完成
            {
                // epoll监听可写事件，如何判断错误码
                m_fd_event->listen(FdEvent::OUT_EVENT, [this, done]()
                                   {
                                       int error = 0;
                                       socklen_t error_len = sizeof(error);
                                       getsockopt(m_fd, SOL_SOCKET, SO_ERROR, &error, &error_len);
                                       bool is_connect_succ = false;
                                       if (error == 0)
                                       {
                                           DEBUGLOG("connect [%s] success", m_peer_addr->toString().c_str());
                                           is_connect_succ = true;
                                           m_connection->setState(Conected);
                                       }
                                       else
                                       {
                                           ERRORLOG("connect error, error=%d, error=%s", errno, strerror(errno));
                                       }
                                       // 连接完后需要去掉可写事件的监听，不然会一直触发
                                       m_fd_event->cancle(FdEvent::OUT_EVENT);
                                       m_event_loop->addEpollEvent(m_fd_event);
                                       //如果连接成功，才会执行回调函数
                                       if (is_connect_succ && done)
                                       {
                                           done();
                                       } });
                m_event_loop->addEpollEvent(m_fd_event);
                if (!m_event_loop->isLooping())
                {
                    m_event_loop->loop();
                }
            }
            else
            {
                ERRORLOG("connect error, error=%d, error=%s", errno, strerror(errno));
            }
        }
    }

    // 异步的发送 Message
    // 如果发送 message成功，会调用done 函数，函数的入参就是message对象
    void TcpClient::writeMessage(AbstractProtocol::s_ptr message, std::function<void(AbstractProtocol::s_ptr)> done)
    {
        // 1.把 message 对象写入 Connection 的 buffer,done 也写入
        // 2.启动connect可写事件
        m_connection->pushSendMessage(message, done);
        m_connection->listenWrite();
    }

    // 异步的读取 Message
    // 如果读取 message成功，会调用done 函数，函数的入参就是message对象
    void TcpClient::readMessage(const std::string &req_id, std::function<void(AbstractProtocol::s_ptr)> done)
    {
        // 1.监听可读事件
        // 2.从buffer里decode 得到message对象,判断是否 req_id相等，相等则读成功，执行其回调
        m_connection->pushReadMessage(req_id, done);
        m_connection->listenRead();
    }
}