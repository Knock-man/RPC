#include <string.h>
#include <fcntl.h>

#include "fd_event.h"
#include "../rocket/common/log.h"
namespace rocket
{
    FdEvent::FdEvent(int fd) : m_fd{fd}
    {
        memset(&m_listen_events, 0, sizeof(m_listen_events));
    }

    FdEvent::FdEvent()
    {
        memset(&m_listen_events, 0, sizeof(m_listen_events));
    }
    FdEvent::~FdEvent()
    {
    }

    void FdEvent::setNonBlock()
    {
        int flag = fcntl(m_fd, F_GETFL, 0);
        if (flag & O_NONBLOCK) // 已经是非阻塞
        {
            return;
        }
        // 设置非阻塞
        fcntl(m_fd, F_SETFL, flag | O_NONBLOCK);
    }

    // 获取该fd读写事件对于的事件处理器
    std::function<void()> FdEvent::handler(TriggerEvent event_type)
    {
        if (event_type == TriggerEvent::IN_EVENT)
        {
            return m_read_callback;
        }
        else
        {
            return m_write_callback;
        }
    }

    // 绑定该fd读写事件对应的事件处理器
    void FdEvent::listen(TriggerEvent event_type, std::function<void()> callback)
    {
        if (event_type == TriggerEvent::IN_EVENT)
        {
            m_listen_events.events |= EPOLLIN;
            m_read_callback = callback;
        }
        else
        {
            m_listen_events.events |= EPOLLOUT;
            m_write_callback = callback;
        }
        m_listen_events.data.ptr = this;
    }

    // 取消该fd对应读写事件对应的监听
    void FdEvent::cancle(TriggerEvent event_type)
    {
        if (event_type == TriggerEvent::IN_EVENT)
        {
            m_listen_events.events &= (~EPOLLIN);
        }
        else
        {
            m_listen_events.events &= (~EPOLLOUT);
        }
    }
};