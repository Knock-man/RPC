// 定时器
#pragma once
#include <map>
#include "../rocket/net/fd_event.h"
#include "../rocket/net/timer_event.h"
namespace rocket
{
    class Timer : public FdEvent
    {
    public:
        Timer();
        ~Timer();

        void addTimerEvent(TimerEvent::s_ptr time_event);    // 添加定时任务
        void deleteTimerEvent(TimerEvent::s_ptr time_event); // 删除定时任务

        void OnTimer(); // 当发生IO事件后,eventloop 会执行这个回调函数

    private:
        void resetArriveTime(); // 重置发生时间

    private:
        // 定时队列 <时间戳，定时事件>
        std::multimap<int64_t, TimerEvent::s_ptr> m_pending_events;
    };
}