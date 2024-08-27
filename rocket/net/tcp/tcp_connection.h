
#pragma once
#include <memory>
#include <queue>
#include "rocket/net/tcp/net_addr.h"
#include "rocket/net/tcp/tcp_buffer.h"
#include "rocket/net/io_thread.h"
#include "rocket/net/tcp/abstract_coder.h"
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
        TcpConection(EventLoop *event_loop, int fd, int buffer_size, NetAddr::s_ptr peer_addr, TcpConnectionType type = TcpConectionByServer);
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

        // 启动监听可写事件
        void listenWrite();

        // 启动监听可读事件
        void listenRead();

        // 压入消息 异步发送
        void pushSendMessage(AbstractProtocol::s_ptr, std::function<void(AbstractProtocol::s_ptr)>);

        // 异步接收
        void pushReadMessage(const std::string &req_id, std::function<void(AbstractProtocol::s_ptr)>);

    private:
        EventLoop *m_event_loop{NULL}; // 该连接对应的event_loop

        NetAddr::s_ptr m_local_addr; // 本地的地址
        NetAddr::s_ptr m_peer_addr;  // 对端服务器地址

        TcpBuffer::s_ptr m_in_buffer;  // 接收缓冲区
        TcpBuffer::s_ptr m_out_buffer; // 发送缓冲区

        FdEvent *m_fd_event{NULL};
        AbstractCoder *m_coder{NULL}; // 编解码器
        TcpState m_state;

        int m_fd{0};

        TcpConnectionType m_connection_type{TcpConectionByServer}; // 连接类型

        // std::pair<AbstractProtocol::s_ptr,std::function<void(AbstractProtocol::s_ptr)>
        // 写消息写回调
        std::vector<std::pair<AbstractProtocol::s_ptr, std::function<void(AbstractProtocol::s_ptr)>>> m_write_dones;

        // 读消息读回调
        //  key 为 req_id
        std::map<std::string, std::function<void(AbstractProtocol::s_ptr)>> m_read_dones;
    };

} // namespace rocket
