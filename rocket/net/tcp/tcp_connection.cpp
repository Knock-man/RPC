#include <unistd.h>
#include "rocket/common/log.h"
#include "rocket/net/tcp/tcp_connection.h"
#include "rocket/net/fd_event_group.h"
#include "rocket/net/string_coder.h"
#include "tcp_connection.h"
namespace rocket
{
    TcpConection::TcpConection(EventLoop *event_loop, int fd, int buffer_size, NetAddr::s_ptr peer_addr, TcpConnectionType type /*= TcpConectionByServer*/)
        : m_event_loop(event_loop), m_peer_addr(peer_addr), m_state(NotConenct), m_fd{fd}, m_connection_type(type)
    {
        m_in_buffer = std::make_shared<TcpBuffer>(buffer_size);
        m_out_buffer = std::make_shared<TcpBuffer>(buffer_size);

        // 该连接对应的事件
        m_fd_event = FdEventGroup::getFdEventGroup()->getFdEvent(fd);
        m_fd_event->setNonBlock(); // 设置非阻塞

        // 客户端 在需要读message的时候再去监听
        if (m_connection_type == TcpConectionByServer)
        {
            listenRead(); // 监听可读事件
        }

        m_coder = new StringCoder();
    }
    TcpConection::~TcpConection()
    {
        if (m_coder)
        {
            delete m_coder;
            m_coder = NULL;
        }
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
            return;
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
        if (m_connection_type == TcpConectionByServer)
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

            listenWrite();
        }
        else
        {
            // 从buffer 里 decode 的到 message 对象，判断是否为req_id 相等，相等则读成功，执行其回调
            std::vector<AbstractProtocol::s_ptr> reuslt; // 临时协议数组
            m_coder->decode(reuslt, m_in_buffer);
            for (size_t i = 0; i < reuslt.size(); i++)
            {
                std::string req_id = reuslt[i]->getReqId(); // 获取协议序列号
                auto it = m_read_dones.find(req_id);
                if (it != m_read_dones.end())
                {
                    it->second(reuslt[i]); // 获取result[i]的智能指针
                }
            }
        }
    }

    void TcpConection::onWrite()
    {
        // 将当前 out_buffer 里面的数据全部发送给client
        if (m_state != Conected)
        {
            ERRORLOG("onWrite error,client has already disconnected, addr[%s],client[%d]", m_peer_addr->toString().c_str(), m_fd);
            return;
        }

        if (m_connection_type == TcpConectionByClient)
        {
            // 1.将message ecode得到字节流
            // 2.字节流写入到 buffer 里面，如何全部发送
            std::vector<AbstractProtocol::s_ptr> messages;
            for (int i = 0; i < m_write_dones.size(); i++)
            {
                messages.push_back(m_write_dones[i].first);
            }
            // 编码进发送缓冲区
            m_coder->encode(messages, m_out_buffer);
        }

        // 取发送缓冲区字节流发送
        bool is_write_all = false;
        while (true)
        {
            if (m_out_buffer->readAble() == 0)
            {
                DEBUGLOG("no data need to send to client [%s]", m_peer_addr->toString().c_str());
                is_write_all = true;
                break;
            }
            int read_size = m_out_buffer->readAble();
            int read_index = m_out_buffer->readIndex();
            int rt = write(m_fd, &(m_out_buffer->m_buffer[read_index]), read_size);
            if (rt >= read_size)
            {
                m_out_buffer->moveReadIndex(rt); // 输出缓冲区可读索引移动
                // DEBUGLOG("no data need to send to client [%s]", m_peer_addr->toString().c_str());
                std::string str(m_out_buffer->m_buffer.begin() + read_index, m_out_buffer->m_buffer.begin() + read_index + read_size);
                INFOLOG("success send request[%s] to client[%s]", str.c_str(), m_peer_addr->toString().c_str());
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
            m_event_loop->addEpollEvent(m_fd_event);
        }

        // 对于客户端，发送完数据之后需要调用回调
        if (m_connection_type == TcpConectionByClient)
        {
            for (size_t i = 0; i < m_write_dones.size(); i++)
            {
                m_write_dones[i].second(m_write_dones[i].first); // 回调函数
            }
            m_write_dones.clear();
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
        m_fd_event->cancle(FdEvent::IN_EVENT);
        m_fd_event->cancle(FdEvent::OUT_EVENT);
        // 从event_loop摘下
        m_event_loop->deleteEpollEvent(m_fd_event);

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

    void TcpConection::setConectionType(TcpConnectionType type)
    {
        m_connection_type = type;
    }

    void TcpConection::listenWrite()
    {
        m_fd_event->listen(FdEvent::OUT_EVENT, std::bind(&TcpConection::onWrite, this));
        m_event_loop->addEpollEvent(m_fd_event);
    }

    void TcpConection::listenRead()
    {
        m_fd_event->listen(FdEvent::IN_EVENT, std::bind(&TcpConection::onRead, this));
        m_event_loop->addEpollEvent(m_fd_event);
    }

    void TcpConection::pushReadMessage(const std::string &req_id, std::function<void(AbstractProtocol::s_ptr)> done)
    {
        m_read_dones.insert(std::make_pair(req_id, done));
    }

    void TcpConection::pushSendMessage(AbstractProtocol::s_ptr message, std::function<void(AbstractProtocol::s_ptr)> done)
    {
        m_write_dones.push_back(std::make_pair(message, done));
    }

} // namespace rocket