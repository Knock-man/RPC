#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "eventloop.h"
#include "../rocket/common/log.h"
#include "../rocket/common/util.h"

// 套接字挂到epoll上(或者修改)
#define ADD_TO_EPOLL()                                                                             \
    auto it = m_listen_fds.find(event->getFd());                                                   \
    int op = EPOLL_CTL_ADD;                                                                        \
    if (it != m_listen_fds.end())                                                                  \
    {                                                                                              \
        op = EPOLL_CTL_MOD;                                                                        \
    }                                                                                              \
    epoll_event tmp = event->getEpollEvent();                                                      \
    int rt = epoll_ctl(m_epoll_fd, op, event->getFd(), &tmp);                                      \
    if (rt == -1)                                                                                  \
    {                                                                                              \
        ERRORLOG("failed epoll_ctl when add fd, errno=%d, errno info=%s", errno, strerror(errno)); \
    }                                                                                              \
    DEBUGLOG("epoll_ctl Add event success, fd[%d]", event->getFd())

// epoll中删除套接字
#define DELETE_TO_EPOOLL()                                                                    \
    auto it = m_listen_fds.find(event->getFd());                                              \
    if (it == m_listen_fds.end())                                                             \
    {                                                                                         \
        return;                                                                               \
    }                                                                                         \
    int op = EPOLL_CTL_DEL;                                                                   \
    epoll_event tmp = event->getEpollEvent();                                                 \
    int rt = epoll_ctl(m_epoll_fd, op, event->getFd(), &tmp);                                 \
    if (rt == -1)                                                                             \
    {                                                                                         \
        ERRORLOG("failed epoll_ctl when add fd, errno=%d, errno=%s", errno, strerror(errno)); \
    }                                                                                         \
    DEBUGLOG("epoll_ctl delete event success, fd[%d]", event->getFd());

namespace rocket
{
    static thread_local EventLoop *t_current_eventloop = NULL;
    static int g_epoll_max_timeout = 100000;
    static int g_epoll_max_events = 10;

    // 构造函数
    EventLoop::EventLoop()
    {
        if (t_current_eventloop != nullptr) // 已经创建eventloop
        {
            ERRORLOG("failed to create event loop, this thread has created event loop");
            exit(0);
        }

        m_loop_threadId = getThreadId(); // 获取当前线程id
        m_epoll_fd = epoll_create(10);   // 创建epoll
        if (m_epoll_fd == -1)
        {
            ERRORLOG("failed to create event loop, m_epoll_fd create error, error info[%d]", errno);
            exit(0);
        }

        initWakeUpFdEvent(); // 初始化wakeup_fd
        INFOLOG("success create event loop in thread %d", m_loop_threadId);
        t_current_eventloop = this;
    }

    EventLoop::~EventLoop()
    {
        close(m_epoll_fd);
        if (m_wakeup_fd_event)
        {
            delete m_wakeup_fd_event;
            m_wakeup_fd_event = NULL;
        }
    }

    // 初始化唤醒套接字
    void EventLoop::initWakeUpFdEvent()
    {
        m_weakup_fd = eventfd(0, EFD_NONBLOCK); // 创建唤醒非阻塞fd

        if (m_weakup_fd < 0)
        {
            ERRORLOG("failed to create event loop, m_weakup_fd create error, error info[%d]", errno);
            exit(0);
        }

        m_wakeup_fd_event = new WakeUpFdEvent(m_weakup_fd); // 创建weakup对象
        m_wakeup_fd_event->listen(FdEvent::IN_EVENT, [this]()
                                  {
                                      char buf[8];
                                      while (read(m_weakup_fd, buf, 8) != -1 && errno != EAGAIN)
                                      {
                                      }
                                       DEBUGLOG("read full bytes from wakeup fd[%d]", m_weakup_fd); });
        DEBUGLOG("m_weakup_fd create success fd=%d", m_weakup_fd);

        addEpollEvent(m_wakeup_fd_event); // 挂到红黑树上
    }

    // 事件循环
    void EventLoop::loop()
    {
        while (!m_stop_flag)
        {
            // 任务队列去任务执行
            std::queue<std::function<void()>> tmp_tasks;
            {
                std::lock_guard<std::mutex> lock(mtx);
                // tmp_tasks = m_pending_task;
                m_pending_task.swap(tmp_tasks);
            }

            while (!tmp_tasks.empty())
            {
                std::function<void()> cb = tmp_tasks.front();
                tmp_tasks.pop();
                if (cb) // 确保回调不为空
                {
                    cb();
                }
            }

            // epoll_wait循环监听事件
            int timeout = g_epoll_max_timeout;
            epoll_event result_events[g_epoll_max_events];
            DEBUGLOG("now begin to epoll_wait");
            int rt = epoll_wait(m_epoll_fd, result_events, g_epoll_max_events, g_epoll_max_timeout);
            DEBUGLOG("now end to epoll_wait,rt=%d", rt);
            if (rt < 0)
            {
                ERRORLOG("epoll_wait error, error=", errno);
            }
            else
            {
                for (int i = 0; i < rt; ++i)
                {
                    epoll_event trrigger_event = result_events[i];                       // 事件
                    FdEvent *fd_event = static_cast<FdEvent *>(trrigger_event.data.ptr); // 获取事件对于的套接字对象
                    if (fd_event == NULL)
                        continue;
                    if (trrigger_event.events & EPOLLIN) // 读事件
                    {
                        DEBUGLOG("fd %d trigger EPOLLIN event", fd_event->getFd());
                        addTask(fd_event->handler(FdEvent::IN_EVENT)); // 套接字的读回调函数添加进任务队列
                    }
                    if (trrigger_event.events & EPOLLOUT)
                    {
                        DEBUGLOG("fd %d trigger EPOLLIN event", fd_event->getFd());
                        addTask(fd_event->handler(FdEvent::OUT_EVENT)); // 套接字的写回调函数添加进任务队列
                    }
                }
            }
        }
    }

    void EventLoop::wakeup()
    {
        m_wakeup_fd_event->wakeup();
    }
    void EventLoop::dealWakeup()
    {
    }

    void EventLoop::stop()
    {
        m_stop_flag = true;
    }
    // epoll_ctl 添加
    void EventLoop::addEpollEvent(FdEvent *event)
    {
        if (isInLoopThread()) // 在当前线程，直接执行
        {
            ADD_TO_EPOLL();
        }
        else //// 不是在当前线程操作，添加进任务队列，等待执行
        {
            auto cb = [this, event]()
            {
                ADD_TO_EPOLL();
            };
            addTask(cb, true);
        }
    }

    // epoll_ctl 删除
    void EventLoop::deleteEpollEvent(FdEvent *event)
    {
        if (isInLoopThread()) // 在当前线程，直接执行
        {
            DELETE_TO_EPOOLL();
        }
        else // 不是在当前线程操作，添加进任务队列，等待执行
        {
            auto cb = [this, event]()
            {
                DELETE_TO_EPOOLL();
            };
            addTask(cb, true);
        }
    }
    // 是否为当前线程
    bool EventLoop::isInLoopThread()
    {
        return getThreadId() == m_loop_threadId;
    }

    // 想任务队列中添加任务
    void EventLoop::addTask(std::function<void()> cb, bool is_wake_up /*==fasle*/)
    {
        {
            std::lock_guard<std::mutex> lock(mtx);
            m_pending_task.push(cb);
        }
        if (is_wake_up) // 是否立马唤醒epoll_wait
        {
            wakeup();
        }
    }
}