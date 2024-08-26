#include "timer_event.h"
#include "../rocket/common/log.h"
#include "../rocket/common/util.h"
namespace rocket
{
    TimerEvent::TimerEvent(int interval, bool is_repeated, std::function<void()> cb)
        : m_interval(interval), m_is_repeated(is_repeated), m_task(cb)
    {
        resetArriveTime();
    }

    // 重置发生时间
    void TimerEvent::resetArriveTime()
    {
        m_arrive_time = getNowMs() + m_interval;
        DEBUGLOG("success create timer event, will excute at [%lld]", m_arrive_time);
    }
}