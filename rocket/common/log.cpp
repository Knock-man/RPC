#include <sys/time.h>
#include <time.h>
#include <sstream>
#include <stdio.h>
#include "log.h"
#include "util.h"

namespace rocket
{
    Logger *Logger::g_logger = nullptr;

    std::string LogLevelToString(LogLevel level)
    {
        std::string LevelString;
        switch (level)
        {
        case Debug:
        {
            LevelString = "Debug";
            break;
        }
        case Info:
        {
            LevelString = "Info";
            break;
        }
        case Error:
        {
            LevelString = "Error";
            break;
        }
        default:
            LevelString = "NONE";
            break;
        }
        return LevelString;
    }

    LogLevel StringToLevel(const std::string level)
    {
        if (level == "Debug")
        {
            return LogLevel::Debug;
        }
        else if (level == "Info")
        {
            return LogLevel::Info;
        }
        else if (level == "Error")
        {
            return LogLevel::Error;
        }
        else
        {
            return LogLevel::NONE;
        }
    }
    std::string LogEvent::toString()
    {
        // 时间
        struct timeval now_time;
        gettimeofday(&now_time, nullptr);

        struct tm now_time_t;
        localtime_r(&(now_time.tv_sec), &now_time_t);

        char buf[128];
        strftime(&buf[0], 128, "%y-%m-%d %H:%M:%S", &now_time_t);

        std::string time_str(buf);

        int ms = now_time.tv_usec / 1000;
        time_str = time_str + "." + std::to_string(ms);

        // 线程进程
        m_pid = getPid();
        m_thread_id = getThreadId();

        // 格式化日志
        std::stringstream ss;
        ss << "[" << LogLevelToString(m_level) << "]\t"
           << "[" << time_str << "]\t"
           << "[pid:" << m_pid << " tid:" << m_thread_id << "]\t";

        return ss.str();
    }

    // 日志入队
    void Logger::pushLog(const std::string &msg)
    {
        std::lock_guard<std::mutex> lock(mtx);
        m_logQue.push(msg);
    }

    void Logger::log()
    {
        // 拷贝日志队列
        std::queue<std::string> tmp;
        {
            std::lock_guard<std::mutex> lock(mtx);
            // tmp = m_logQue;
            m_logQue.swap(tmp);
        }
        // 输出日志
        while (!tmp.empty())
        {
            {

                std::string msg = tmp.front();
                tmp.pop();
                printf("%s", msg.c_str());
            }
        }
    }

}