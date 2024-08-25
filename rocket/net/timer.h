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
        void resetArriveTime();

    private:
        std::multimap<int64_t, TimerEvent::s_ptr> m_pending_events;
    };
}