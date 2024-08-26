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
    class TcpConection
    {
    public:
        typedef std::shared_ptr<TcpConection> s_ptr;
        TcpConection(IOThread *io_thread, int fd, int buffer_size, NetAddr::s_ptr peer_addr);
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

    private:
        IOThread *m_io_thread{NULL}; // 代表持有该连接的IO线程

        NetAddr::s_ptr m_local_addr; // 本地的地址
        NetAddr::s_ptr m_peer_addr;  // 对端服务器地址

        TcpBuffer::s_ptr m_in_buffer;  // 接收缓冲区
        TcpBuffer::s_ptr m_out_buffer; // 发送缓冲区

        FdEvent *m_fd_event{NULL};

        TcpState m_state;

        int m_fd{0};
    };

} // namespace rocket
