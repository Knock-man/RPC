#include <sys/time.h>
#include <time.h>
#include <sstream>
#include <stdio.h>
#include <thread>

#include "rocket/net/eventloop.h"
#include "log.h"
#include "util.h"

namespace rocket
{
    Logger *Logger::g_logger = nullptr;

    Logger::Logger(LogLevel level) : m_set_level(level) {

                                     };

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
           << "[" << m_pid << ":" << m_thread_id << "]\t";

        return ss.str();
    }

    // 日志入队
    void Logger::pushLog(const std::string &msg)
    {
        printf("%s", msg.c_str());
        std::lock_guard<std::mutex> lock(mtx);
        m_logQue.push_back(msg);
    }

    void Logger::log()
    {
        // // 拷贝日志队列
        // std::queue<std::string> tmp;
        // {
        //     std::lock_guard<std::mutex> lock(mtx);
        //     // tmp = m_logQue;
        //     m_logQue.swap(tmp);
        // }
        // // 输出日志
        // while (!tmp.empty())
        // {
        //     {

        //         std::string msg = tmp.front();
        //         tmp.pop();
        //         printf("%s", msg.c_str());
        //     }
        // }
    }

    // 同步m_buf到async_logger 的buffer队尾
    void Logger::syncLoop()
    {
        // printf("同步异步队列\n");
        std::vector<std::string> tmp_vec;
        {
            std::lock_guard<std::mutex> lock(mtx);
            m_logQue.swap(tmp_vec);
        }
        // printf("同步队列大小%d\n", tmp_vec.size());
        if (!tmp_vec.empty())
        {
            m_asnyc_logger->pushLogBuffer(tmp_vec);
        }
    }

    void Logger::init()
    {
        m_asnyc_logger = std::make_shared<AsynLogger>(
            Config::GetGlobalConfig()->m_log_file_name,
            Config::GetGlobalConfig()->m_log_file_path,
            Config::GetGlobalConfig()->m_log_max_file_size);

        m_timer_event = std::make_shared<TimerEvent>(Config::GetGlobalConfig()->m_log_sync_inteval, true, std::bind(&Logger::syncLoop, this));

        EventLoop::GetCurrentEventLoop()->addTimerEvent(m_timer_event);
        // printf("安装定时器成功，时间间隔%d\n", Config::GetGlobalConfig()->m_log_sync_inteval);
    }

    AsynLogger::AsynLogger(std::string file_name, std::string file_path, int max_size)
        : m_file_name(file_name), m_file_path(file_path), m_max_file_size(max_size)
    {
        printf("构造异步对象\n");
        std::thread t(loop, this);
        {
            std::unique_lock<std::mutex> lock(mtx);
            m_sempahore.wait(lock);
        }
        t.detach();
        printf("异步对象构造完毕\n");
    }

    // 将buffer里面的全部数据打印到文件中，然后线程睡眠，直到有新的数据再重复这个过程
    void AsynLogger::loop(AsynLogger *arg)
    {

        AsynLogger *thiz = (AsynLogger *)arg;
        thiz->m_sempahore.notify_one(); // 通知构造函数线程启动完毕
        printf("唤醒异步线程\n");

        while (1)
        {
            std::vector<std::string> tmp;
            { // 等待队列不为空，取队列日志
                std::unique_lock<std::mutex> lock(mtx);
                // printf("等待日志写入\n");
                thiz->m_condition.wait(lock, [thiz]()
                                       { return !thiz->m_buffer.empty(); });
                tmp.swap(thiz->m_buffer.front());
                thiz->m_buffer.pop();
            }
            // printf("日志写入\n");
            timeval now;
            gettimeofday(&now, NULL);

            struct tm now_time;
            localtime_r((&now.tv_sec), &now_time);

            const char *format = "%Y%m%d";
            char date[32];
            strftime(date, sizeof(date), format, &now_time);

            // 当前日志不等于上次一打印的日期  需要重新打开一个日志文件
            if (std::string(date) != thiz->m_date)
            {
                thiz->m_no = 0;
                thiz->m_reopen_flag = true;
                thiz->m_date = std::string(date);
            }
            if (thiz->m_file_handler == NULL)
            {
                thiz->m_reopen_flag = true;
            }

            // 生成日志文件名
            std::stringstream ss;
            ss << thiz->m_file_path << thiz->m_file_name << "_" << std::string(date) << "_rpc_log.";
            std::string log_file_name = ss.str() + std::to_string(thiz->m_no);

            if (thiz->m_reopen_flag) // 需要重新打开新的日志文件
            {
                if (thiz->m_file_handler) // 关闭当前日志文件
                {
                    fclose(thiz->m_file_handler);
                }
                // 打开新的日志文件
                thiz->m_file_handler = fopen(log_file_name.c_str(), "a");
                if (!thiz->m_file_handler)
                {
                    printf("打开日志文件失败");
                    continue;
                }
                thiz->m_reopen_flag = false;
            }

            if (ftell(thiz->m_file_handler) > thiz->m_max_file_size) // 当前文件字节数大于最大文件字节数
            {
                fclose(thiz->m_file_handler);
                if (!thiz->m_file_handler)
                {
                    printf("打开日志文件失败");
                    continue;
                }
                thiz->m_no++;
                log_file_name = ss.str() + std::to_string(thiz->m_no++); // 新的文件名
                // 打开新的日志文件
                thiz->m_file_handler = fopen(log_file_name.c_str(), "a");
                thiz->m_reopen_flag = false;
            }

            // 写入文件落盘
            for (auto &i : tmp)
            {
                if (!i.empty())
                {
                    fwrite(i.c_str(), 1, i.length(), thiz->m_file_handler);
                }
            }
            fflush(thiz->m_file_handler);

            if (thiz->m_stop_flag) // 关闭loop
            {
                return;
            }
        }
    }

    void AsynLogger::stop()
    {
        m_stop_flag = true;
    }

    void AsynLogger::flush()
    {
        if (m_file_handler)
        {
            fflush(m_file_handler);
        }
    }
    void AsynLogger::pushLogBuffer(std::vector<std::string> &vec)
    {
        {
            std::lock_guard<std::mutex> lock(mtx);
            m_buffer.push(vec);
        }
        // 这时候需要唤醒异步日志线程
        m_condition.notify_one();
    }
}