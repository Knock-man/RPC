#include <unistd.h>
#include "rocket/net/tcp/tcp_connection.h"
#include "rocket/net/fd_event_group.h"
#include "rocket/common/log.h"
#include "tcp_connection.h"
namespace rocket
{
    TcpConection::TcpConection(IOThread *io_thread, int fd, int buffer_size, NetAddr::s_ptr peer_addr)
        : m_io_thread(io_thread), m_peer_addr(peer_addr), m_state(NotConenct), m_fd{fd}
    {
        m_in_buffer = std::make_shared<TcpBuffer>(buffer_size);
        m_out_buffer = std::make_shared<TcpBuffer>(buffer_size);

        // 该连接对应的事件
        m_fd_event = FdEventGroup::getFdEventGroup()->getFdEvent(fd);
        m_fd_event->setNonBlock(); // 设置非阻塞
        m_fd_event->listen(FdEvent::IN_EVENT, std::bind(&TcpConection::onRead, this));

        io_thread->getEventLoop()->addEpollEvent(m_fd_event);
    }
    TcpConection::~TcpConection()
    {
    }

    void TcpConection::onRead()
    {
        // 1.从socket缓冲区调用系统的read 函数读取
        if (m_state != Conected)
        {
            ERRORLOG("onRead error,client has already disconnected, addr[%s],client[%d]", m_peer_addr->toString().c_str(), m_fd);
            return;
        }
        bool is_read_all = false;
        bool is_close = false;
        while (!is_read_all)
        {
            if (m_in_buffer->writeAble() == 0) // 扩容
            {
                m_in_buffer->resizeBuffer(2 * m_in_buffer->m_buffer.size());
            }

            int read_count = m_in_buffer->writeAble();   // 可写字节数
            int write_index = m_in_buffer->writeIndex(); // 可写索引
            printf("read_count=%d write_index=%d\n", read_count, write_index);
            int rt = read(m_fd, &(m_in_buffer->m_buffer[write_index]), read_count);
            DEBUGLOG("success read %d bytes from addr[%s],client fd[%d]", rt, m_peer_addr->toString().c_str(), m_fd);
            if (rt > 0)
            {
                m_in_buffer->moveWriteIndex(rt); // 写缓冲区可写索引移动

                if (rt == read_count) // 数据没有读完,缓冲区需要扩容
                {
                    continue;
                }
                else if (rt < read_count) // 数据读完了
                {
                    is_read_all = true;
                    break;
                }
            }
            else if (rt == 0) // 对端关闭
            {
                is_close = true;
                break;
            }
            else if (rt == -1 && errno == EAGAIN) // 非阻塞没有数据可读
            {
                is_read_all = true;
                break;
            }
        }
        if (is_close)
        {
            DEBUGLOG("peer closed, peer addr [%d], clientfd [%d]", m_peer_addr->toString().c_str(), m_fd);
            clear(); // 关闭连接
        }

        if (!is_read_all)
        {
            ERRORLOG("not read all data");
        }

        // TODO 简单的echo,后面补充RPC协议解析
        excute();
    }
    void TcpConection::excute()
    {
        // 将 RPC 请求执行业务逻辑，获取 RPC 响应，再把 RPC 响应发送回去
        std::vector<char> tmp;
        int size = m_in_buffer->readAble();
        tmp.resize(size);

        // 读取缓冲区size大小的数据
        m_in_buffer->readFromBuffer(tmp, size);

        std::string msg;
        for (size_t i = 0; i < tmp.size(); i++)
        {
            msg += tmp[i];
        }

        INFOLOG("success get request[%s] from client[%s]", msg.c_str(), m_peer_addr->toString().c_str());

        m_out_buffer->writeToBuffer(msg.c_str(), msg.length());

        m_fd_event->listen(FdEvent::OUT_EVENT, std::bind(&TcpConection::onWrite, this));
        m_io_thread->getEventLoop()->addEpollEvent(m_fd_event);
    }

    void TcpConection::onWrite()
    {
        // 将当前 out_buffer 里面的数据全部发送给client
        if (m_state != Conected)
        {
            ERRORLOG("onWrite error,client has already disconnected, addr[%s],client[%d]", m_peer_addr->toString().c_str(), m_fd);
            return;
        }

        bool is_write_all = false;
        while (true)
        {
            if (m_out_buffer->readAble() == 0)
            {
                DEBUGLOG("no data need to send to client [%s]", m_peer_addr->toString().c_str());
                is_write_all = true;
                break;
            }
            int write_size = m_out_buffer->readAble();
            int read_index = m_out_buffer->readIndex();
            int rt = write(m_fd, &(m_out_buffer->m_buffer[read_index]), write_size);
            if (rt >= write_size)
            {
                m_out_buffer->moveReadIndex(rt); // 输出缓冲区可读索引移动
                DEBUGLOG("no data need to send to client [%s]", m_peer_addr->toString().c_str());
                is_write_all = true;
                break;
            }
            else if (rt == -1 && errno == EAGAIN) // 内核发送缓冲区已经满,不能在发送
            {
                // 这种情况等待下一次 fd 可写的适合再次发送数据即可 fd可写触发epoll_wait
                ERRORLOG("write data error, error=EAGIN and rt == -1");
                break;
            }
        }
        if (is_write_all)
        {
            m_fd_event->cancle(FdEvent::OUT_EVENT);
            m_io_thread->getEventLoop()->addEpollEvent(m_fd_event);
        }
    }

    void TcpConection::setState(const TcpState state)
    {
        m_state = state;
    }
    TcpState TcpConection::getstate() const
    {
        return m_state;
    }
    void TcpConection::clear()
    {
        // 服务器处理一些关闭连接后的清理动作
        if (m_state == Closed)
        {
            return;
        }
        // 从event_loop摘下
        m_io_thread->getEventLoop()->deleteEpollEvent(m_fd_event);

        m_state = Closed;
    }

    void TcpConection::shutdown()
    {
        if (m_state == Closed || m_state == NotConenct)
        {
            return;
        }
        m_state = HalfCloseing; // 半关闭连接

        // 调用shutdown 关闭读写,意味着服务器不会再对这个 fd 进行读写操作了
        // 发送 FIN 报文，触发了四次挥手的第一个阶段
        // 当fd发生可读事件，但是可读的数据为0，即 对端发送了FIN报文
        ::shutdown(m_fd, SHUT_RDWR);
    }

} // namespace rocket