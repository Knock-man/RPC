#include "rocket/net/tcp/tcp_client.h"
#include "rocket/common/log.h"
#include "rocket/net/fd_event_group.h"
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include "tcp_client.h"
#include "rocket/common/error_code.h"
namespace rocket
{
    // 构造函数 参数为服务器地址
    TcpClient::TcpClient(NetAddr::s_ptr peer_addr) : m_peer_addr(peer_addr)
    {
        // 获取当前线程eventloop
        m_event_loop = EventLoop::GetCurrentEventLoop();
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
        m_connection = std::make_shared<TcpConection>(m_event_loop, m_fd, 128, peer_addr, nullptr, TcpConectionByClient);
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
        // 连接服务器
        int rt = ::connect(m_fd, m_peer_addr->getSockAddr(), m_peer_addr->getSockLen());
        if (rt == 0)
        {
            DEBUGLOG("connect [%s] success", m_peer_addr->getSockAddr(), m_peer_addr->getSockLen());
            initLocalAddr();
            m_connection->setState(Conected);
            if (done)
                done();
        }
        else if (rt == -1) // 注意可能正在三次握手的过程中
        {
            if (errno == EINPROGRESS) // 异步非阻塞connect 连接请求已经开始，连接尚未完成
            {
                // epoll监听可写事件，如何判断错误码
                m_fd_event->listen(FdEvent::OUT_EVENT, [this, done]() // 前两次握手成功，触发客户端套接字可写事件
                                   {    
                                       int error = 0;
                                       socklen_t error_len = sizeof(error);
                                       getsockopt(m_fd, SOL_SOCKET, SO_ERROR, &error, &error_len);
                                     
                                       if (error == 0)
                                       {
                                           DEBUGLOG("connect [%s] success", m_peer_addr->toString().c_str());
                                           initLocalAddr();
                                           m_connection->setState(Conected);
                                       }
                                       else
                                       {
                                           m_connect_err_code = ERROR_FAILED_CONNECT;
                                           m_connect_error_info = "connect error,sys error=" + std::string(strerror(errno));
                                           ERRORLOG("connect error, error=%d, error=%s", errno, strerror(errno));
                                       }
                                       // 连接完后需要去掉可写事件的监听，不然会一直触发
                                       m_event_loop->deleteEpollEvent(m_fd_event);
                                       //    m_fd_event->cancle(FdEvent::OUT_EVENT);
                                       //    m_event_loop->addEpollEvent(m_fd_event);
                                       // 如果连接成功，才会执行回调函数
                                       if (done)
                                       {
                                           done();
                                       } }, [this, done]()                // 注册错误事件回调
                                   {
                                    //     m_fd_event->cancle(FdEvent::OUT_EVENT);
                                    //    m_event_loop->addEpollEvent(m_fd_event);
                                        if(errno==ECONNREFUSED){//连接失败，对端没有服务器监听
                                                m_connect_err_code = ERROR_FAILED_CONNECT;
                                                m_connect_error_info = "connect refused,sys error=" + std::string(strerror(errno));
                                                
                                        }else{
                                            m_connect_err_code = ERROR_FAILED_CONNECT;
                                            m_connect_error_info = "connect unkonwn,sys error=" + std::string(strerror(errno));
                                        }
                                        ERRORLOG("connect error, error=%d, error=%s", errno, strerror(errno)); });
                m_event_loop->addEpollEvent(m_fd_event);
                if (!m_event_loop->isLooping())
                {
                    m_event_loop->loop();
                }
            }
            else
            {

                m_connect_err_code = ERROR_FAILED_CONNECT;
                m_connect_error_info = "connect error,sys error=" + std::string(strerror(errno));
                ERRORLOG("connect error, error=%d, error=%s", errno, strerror(errno));
                if (done)
                {
                    done();
                }
            }
        }
    }

    // 发送消息，提供消息+发送完成异步通知函数
    //  异步的发送 Message
    //  如果发送 message成功，会调用done 函数，函数的入参就是message对象
    void TcpClient::writeMessage(AbstractProtocol::s_ptr message, std::function<void(AbstractProtocol::s_ptr)> done)
    {
        // 1.把 message 对象写入 Connection 的 buffer,done 也写入
        // 2.启动connect可写事件
        m_connection->pushSendMessage(message, done); // 注册消息和通知函数
        m_connection->listenWrite();                  // 提交可写事件
    }

    // 接收消息，提供消息ID+接收完成异步通知函数
    // 异步的读取 Message
    // 如果读取 message成功，会调用done 函数，函数的入参就是message对象
    void TcpClient::readMessage(const std::string &msg_id, std::function<void(AbstractProtocol::s_ptr)> done)
    {
        // 1.监听可读事件
        // 2.从buffer里decode 得到message对象,判断是否 msg_id相等，相等则读成功，执行其回调
        m_connection->pushReadMessage(msg_id, done); // 注册消息和通知函数
        m_connection->listenRead();                  // 提交可读事件
    }
    void TcpClient::stop()
    {
        if (m_event_loop->isLooping())
        {
            m_event_loop->stop();
        }
    }

    int TcpClient::getConnectErrorCode()
    {
        return m_connect_err_code;
    }

    std::string TcpClient::getConnectErrorInfo()
    {
        return m_connect_error_info;
    }

    NetAddr::s_ptr TcpClient::getPeerAddr()
    {
        return m_peer_addr;
    }
    NetAddr::s_ptr TcpClient::getlocalAddr()
    {
        return m_local_addr;
    }
    void TcpClient::initLocalAddr()
    {
        sockaddr_in local_addr;
        socklen_t len = sizeof(local_addr);

        int ret = getsockname(m_fd, reinterpret_cast<sockaddr *>(&local_addr), &len);
        if (ret != 0)
        {
            ERRORLOG("initLocalAddr error, getsockname error, errno=%d, error=%s", errno, strerror(errno));
            return;
        }

        m_local_addr = std::make_shared<IPNetAddr>(local_addr);
    }

    void TcpClient::addTimerEvent(TimerEvent::s_ptr timer_event)
    {
        m_event_loop->addTimerEvent(timer_event);
    }
}
