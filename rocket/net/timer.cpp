#include <sys/timerfd.h>
#include <string.h>
#include "timer.h"
#include "../rocket/common/log.h"
#include "../rocket/common/util.h"

namespace rocket
{
    Timer::Timer() : FdEvent()
    {
        // 创建定时fd  时间到了变为可变事件
        m_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
        DEBUGLOG("time fd = %d", m_fd);

        // 设置可读事件 绑定读事件处理函数
        listen(FdEvent::IN_EVENT, std::bind(&Timer::OnTimer, this));
    }

    Timer::~Timer()
    {
    }
    // 添加定时任务
    void Timer::addTimerEvent(TimerEvent::s_ptr time_event)
    {
        bool is_reset_timerfd = false; // 是否重写设置超时时间(防止加入的任务时间已经过时，但没有处理)
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (m_pending_events.empty()) // 定时队列为空，重置定时时间
            {
                is_reset_timerfd = true;
            }
            else
            {
                // 当前插入事件的发生时间 早于 定时队列所有事件的发生时间，需要重置发生时间(防止已经超时未处理)
                auto it = m_pending_events.begin();
                if ((*it).second->getArriveTime() > time_event->getArriveTime())
                {
                    is_reset_timerfd = true;
                }
            }
            m_pending_events.emplace(time_event->getArriveTime(), time_event);
        }

        if (is_reset_timerfd) // 重置超时时间
        {
            resetArriveTime();
        }
    }

    // 删除定时任务
    void Timer::deleteTimerEvent(TimerEvent::s_ptr time_event)
    {
        time_event->setCancled(true);
        std::lock_guard<std::mutex> lock(mtx);
        auto begin = m_pending_events.lower_bound(time_event->getArriveTime());
        auto end = m_pending_events.upper_bound(time_event->getArriveTime());

        auto it = begin;
        for (; it != end; it++)
        {
            if (it->second == time_event)
                break;
        }
        if (it != end)
        {
            m_pending_events.erase(it);
        }
        DEBUGLOG("success delete TimeEvent at arrive time %lld", time_event->getArriveTime());
    }

    // 定时器到期处理函数 取出所有超时的任务，进行处理(重复性定时任务需要再次加入定时队列中)
    void Timer::OnTimer()
    {
        // 读取缓冲区数据，防止下一次继续触发可读事件
        char buf[8];
        while (1)
        {
            if ((read(m_fd, buf, 8) == -1) && errno == EAGAIN)
            {
                break;
            }
        }
        // 执行定时任务
        std::vector<TimerEvent::s_ptr> tmps; // 要执行的定时事件
        std::vector<std::pair<int64_t, std::function<void()>>> tasks;

        {
            std::lock_guard<std::mutex> lock(mtx);
            // 取出超时的任务
            auto it = m_pending_events.begin();
            for (it = m_pending_events.begin(); it != m_pending_events.end(); ++it)
            {
                if ((*it).first <= getNowMs()) // 事件已经到期
                {
                    if (!(*it).second->isCancled()) // 事件没有取消
                    {
                        // 加入执行队列
                        tmps.push_back((*it).second);
                        tasks.push_back(std::make_pair((*it).second->getArriveTime(), (*it).second->getCallBack()));
                    }
                }
                else // 事件还未到期,跳出遍历
                {
                    break;
                }
            }

            // 删除超时的任务
            m_pending_events.erase(m_pending_events.begin(), it);
        } // unlock()

        // 需要把周期定时的Event 再次添加定时队列中进去
        for (auto i = tmps.begin(); i != tmps.end(); ++i)
        {
            if ((*i)->isRepeated()) // 是周期性任务
            {
                (*i)->resetArriveTime(); // 重置该任务发生时间
                addTimerEvent(*i);       // 添加进定时队列(会重新排序)
            }
        }

        resetArriveTime(); // 重置定时器套接字可读时间

        // 执行到期定时任务
        for (auto i : tasks)
        {
            if (i.second)
            {
                i.second();
            }
        }
    }

    // 重新设置超时时间
    void Timer::resetArriveTime()
    {
        std::multimap<int64_t, rocket::TimerEvent::s_ptr> tmp;
        {
            std::lock_guard<std::mutex> lock(mtx);
            tmp = m_pending_events;
        }
        if (!tmp.size())
            return;

        int64_t nowTime = getNowMs(); // 获取当前时间戳

        auto it = tmp.begin();
        int64_t inteval = 0;

        if (it->second->getArriveTime() > nowTime) // 最近的定时任务还没有到期
        {
            inteval = it->second->getArriveTime() - nowTime; // 发生间隔设为距离第一个任务时间戳的间隔
        }
        else // 最近的定时任务已经到期了，但还没有执行，立马触发epoll_wait，拯救过期任务
        {
            inteval = 100;
        }

        // 计算下一次触发时间
        timespec ts;
        memset(&ts, 0, sizeof(ts));
        ts.tv_sec = inteval / 1000;
        ts.tv_nsec = (inteval % 1000) * 1000000;

        itimerspec value;
        memset(&value, 0, sizeof(value));
        value.it_value = ts;

        int rt = timerfd_settime(m_fd, 0, &value, NULL); // 重新设置定时器描述符可读时间
        if (rt != 0)
        {
            ERRORLOG("timerfd_settime error,error=%d,error=%s", errno, strerror(errno));
        }
        DEBUGLOG("timer reset to %lld", nowTime + inteval);
    }

} // namespace rocket
