#pragma once
#include <thread>
#include <set>
#include <functional>
#include <queue>

#include "fd_event.h"
#include "wakeup_fd_event.h"
#include "timer.h"

namespace rocket
{
    class EventLoop
    {
    public:
        EventLoop();

        ~EventLoop();

        // loop循环
        void loop();

        // 唤醒wakeup套接字 变为可读
        void wakeup();

        void stop();

        // 添加事件
        void addEpollEvent(FdEvent *event);

        // 删除事件
        void deleteEpollEvent(FdEvent *event);

        // 是否为当前线程
        bool isInLoopThread();

        // 添加任务
        void addTask(std::function<void()> cb, bool is_wake_up = false);

        // 添加定时事件
        void addTimerEvent(TimerEvent::s_ptr event);

        bool isLooping();

    public:
        // 获取当前线程的eventloop
        static EventLoop *GetCurrentEventLoop();

    private:
        void dealWakeup();

        void initWakeUpFdEvent(); // 初始唤醒事件 唤醒套接字添加进epoll

        void initTimer(); // 初始化定时器 定时套接字添加进epoll

    private:
        pid_t m_loop_threadId; // eventloop线程id

        int m_epoll_fd{0}; // epoll 句柄

        int m_weakup_fd{0}; // 唤醒epoll_wait fd

        WakeUpFdEvent *m_wakeup_fd_event{NULL}; // 唤醒事件

        bool m_stop_flag{false}; // 是否停止

        std::set<int> m_listen_fds; // 监听套接字集合

        std::queue<std::function<void()>> m_pending_task; // 待执行的任务队列

        Timer *m_timer{NULL}; // 定时器

        bool m_is_looping{false};
    };
}