#pragma once
#include <memory>
#include "rocket/net/tcp/net_addr.h"
#include "rocket/net/tcp/tcp_buffer.h"
#include "rocket/net/io_thread.h"
namespace rocket
{
    enum TcpState
    {
        NotConenct = 1,
        Conected = 2,
        HalfCloseing = 3,
        Closed = 4
    };
    enum TcpConnectionType
    {
        TcpConectionByServer = 1, // 作为服务器端使用，代表和对端客户端连接
        TcpConectionByClient = 2  // 作为客户端使用，代表和对端服务端连接
    };
    class TcpConection
    {
    public:
        typedef std::shared_ptr<TcpConection> s_ptr;
        TcpConection(EventLoop *event_loop, int fd, int buffer_size, NetAddr::s_ptr peer_addr);
        ~TcpConection();

        void onRead();
        void excute();
        void onWrite();
        void setState(const TcpState state);
        TcpState getstate() const;

        // 关闭连接
        void clear();

        // 服务器主动关闭连接
        void shutdown();

        void setConectionType(TcpConnectionType type);

    private:
        EventLoop *m_event_loop{NULL};

        NetAddr::s_ptr m_local_addr; // 本地的地址
        NetAddr::s_ptr m_peer_addr;  // 对端服务器地址

        TcpBuffer::s_ptr m_in_buffer;  // 接收缓冲区
        TcpBuffer::s_ptr m_out_buffer; // 发送缓冲区

        FdEvent *m_fd_event{NULL};

        TcpState m_state;

        int m_fd{0};

        TcpConnectionType m_connection_type{TcpConectionByServer}; // 连接类型
    };

} // namespace rocket
