#pragma once
#include <functional>
#include <memory>

namespace rocket
{

    class TimerEvent
    {
    public:
        typedef std::shared_ptr<TimerEvent> s_ptr;

        TimerEvent(int interval, bool is_repeated, std::function<void()> cb);

        // 获取发生事件
        int64_t getArriveTime() const
        {
            return m_arrive_time;
        }
        // 设置取消
        void setCancled(bool value)
        {
            m_is_cancled = value;
        }
        // 是否取消
        bool isCancled() const
        {
            return m_is_cancled;
        }
        // 是否重复
        bool isRepeated() const
        {
            return m_is_repeated;
        }
        // 获取回调
        std::function<void()> getCallBack() const
        {
            return m_task;
        }

        void resetArriveTime();

    private:
        int64_t m_arrive_time;     // 下次执行任务的时间点 ms
        int64_t m_interval;        // 定时间隔
        bool m_is_repeated{false}; // 周期性
        bool m_is_cancled{false};  // 取消标志

        std::function<void()> m_task; // 任务
    };
}