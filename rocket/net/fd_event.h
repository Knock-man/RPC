// FDEVENT封装套接字  读写回调
#pragma once
#include <functional>
#include <sys/epoll.h>
namespace rocket
{
    class FdEvent
    {
    public:
        enum TriggerEvent
        {
            IN_EVENT = EPOLLIN,   // 读事件
            OUT_EVENT = EPOLLOUT, // 写事件
            ERROR_EVENT = EPOLLERR,
        };

        FdEvent(int fd);

        FdEvent();

        ~FdEvent();

        void setNonBlock();

        // 获取该fd读写事件对于的事件处理器
        std::function<void()> handler(TriggerEvent event_type);

        // 绑定该fd读写事件对应的事件处理器
        void listen(TriggerEvent event_type, std::function<void()> callback, std::function<void()> error_callback = nullptr);

        // 取消该fd对应读写事件对应的监听
        void cancle(TriggerEvent event_type);

        int getFd() const
        {
            return m_fd;
        }

        // 获取该fd的事件
        epoll_event getEpollEvent() const
        {
            return m_listen_events;
        }

    protected:
        int m_fd{-1}; // 套接字
        epoll_event m_listen_events;
        std::function<void()> m_read_callback{nullptr};  // 读回调
        std::function<void()> m_write_callback{nullptr}; // 写回调
        std::function<void()> m_error_callback{nullptr};
    };

}; // namespace rocket
