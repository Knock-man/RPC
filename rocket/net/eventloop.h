#pragma once
#include <thread>
#include <set>
#include <functional>
#include <queue>

#include "fd_event.h"
#include "wakeup_fd_event.h"

namespace rocket
{
    class EventLoop
    {
    public:
        EventLoop();

        ~EventLoop();

        void loop();

        void wakeup();

        void stop();

        void addEpollEvent(FdEvent *event);

        void deleteEpollEvent(FdEvent *event);

        bool isInLoopThread();

        void addTask(std::function<void()> cb, bool is_wake_up = false);

    private:
        void dealWakeup();

        void initWakeUpFdEvent();

    private:
        pid_t m_loop_threadId; // eventloop线程id

        int m_epoll_fd{0}; // epoll 句柄

        int m_weakup_fd{0}; // 唤醒epoll_wait fd

        WakeUpFdEvent *m_wakeup_fd_event{NULL};

        bool m_stop_flag{false};

        std::set<int> m_listen_fds; // 监听套接字集合

        std::queue<std::function<void()>> m_pending_task; // 待执行的任务队列
    };
}