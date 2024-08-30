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
    if (op == EPOLL_CTL_ADD)                                                                       \
        m_listen_fds.insert(event->getFd());                                                       \
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
    m_listen_fds.erase(event->getFd());                                                       \
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

        initTimer(); // 初始化定时器

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
        if (m_timer)
        {
            delete m_timer;
            m_timer = NULL;
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

    // 添加定时事件
    void EventLoop::addTimerEvent(TimerEvent::s_ptr event)
    {
        m_timer->addTimerEvent(event);
    }

    // 初始化定时器套接字
    void EventLoop::initTimer()
    {
        m_timer = new Timer();
        addEpollEvent(m_timer);
    }

    // 事件循环
    void EventLoop::loop()
    {
        m_is_looping = true;
        while (!m_stop_flag)
        {
            // 任务队列取任务执行
            std::queue<std::function<void()>> tmp_tasks;
            {
                std::lock_guard<std::mutex> lock(mtx);
                m_pending_task.swap(tmp_tasks);
            }

            while (!tmp_tasks.empty()) // 挨个执行任务
            {
                std::function<void()> cb = tmp_tasks.front();
                tmp_tasks.pop();
                if (cb) // 确保回调不为空
                {
                    cb();
                }
            }

            // 如果有定时任务需要执行，那么执行
            // 1怎么判断定时任务需要执行?(now() > TimeEvent.arrtive_time)
            // 2 arrtive_time 如何让eventloop监听

            // epoll_wait循环监听事件
            int timeout = g_epoll_max_timeout;
            epoll_event result_events[g_epoll_max_events]; // events数组
            DEBUGLOG("now begin to epoll_wait");
            int rt = epoll_wait(m_epoll_fd, result_events, g_epoll_max_events, g_epoll_max_timeout);
            DEBUGLOG("now end to epoll_wait,rt=%d", rt);
            if (rt < 0)
            {
                ERRORLOG("epoll_wait error, error=", errno);
            }
            else // 有感兴趣事件发生
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

    // 唤醒wakeup_fd
    void EventLoop::wakeup()
    {
        m_wakeup_fd_event->wakeup();
    }

    // 获得当前线程的event_loop()
    EventLoop *EventLoop::GetCurrentEventLoop()
    {
        if (t_current_eventloop)
        {
            return t_current_eventloop;
        }
        t_current_eventloop = new EventLoop();
        return t_current_eventloop;
    }
    void EventLoop::dealWakeup()
    {
    }

    void EventLoop::stop()
    {
        m_stop_flag = true;
        wakeup(); // 防止退出延时，while(flag){}  需要等待epoll_wait返回
    }
    // epoll_ctl 添加/修改事件
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

    // 向任务队列中添加任务
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
    bool EventLoop::isLooping()
    {
        return m_is_looping;
    }
}