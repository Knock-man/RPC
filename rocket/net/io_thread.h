// IO线程
#pragma once
#include "../rocket/net/eventloop.h"
#include <pthread.h>
#include <condition_variable>
namespace rocket
{
    class IOThread
    {
    public:
        IOThread();

        ~IOThread();

        EventLoop *getEventLoop() const
        {
            return m_event_loop;
        }

        void start();

        void join();

    public:
        static void *Main(void *arg);

    private:
        pid_t m_thread_id{-1};         // 线程号
        pthread_t m_thread{0};         // 线程句柄
        EventLoop *m_event_loop{NULL}; // 当前 IO 线程的event_loop对象

        // 信号量
        std::condition_variable m_init_semaphore;

        std::condition_variable m_start_semaphore;
    };
}