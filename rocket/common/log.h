#pragma once
#include <string>
#include <memory>
#include <queue>
#include <iostream>
#include <condition_variable>
#include <mutex>

#include "rocket/net/timer_event.h"
#include "config.h"
#include "util.h"

#define DEBUGLOG(str, ...)                                                                                                                                                                                                           \
    if (rocket::Logger::GetGlobolLogger()->getLoglevel() <= rocket::Debug)                                                                                                                                                           \
    {                                                                                                                                                                                                                                \
        rocket::Logger::GetGlobolLogger()->pushLog((rocket::LogEvent(rocket::LogLevel::Debug).toString()) + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + '\n'); \
    }

#define INFOLOG(str, ...)                                                                                                                                                                                                           \
    if (rocket::Logger::GetGlobolLogger()->getLoglevel() <= rocket::Info)                                                                                                                                                           \
    {                                                                                                                                                                                                                               \
        rocket::Logger::GetGlobolLogger()->pushLog((rocket::LogEvent(rocket::LogLevel::Info).toString()) + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + '\n'); \
    }

#define ERRORLOG(str, ...)                                                                                                                                                                                                           \
    if (rocket::Logger::GetGlobolLogger()->getLoglevel() <= rocket::Error)                                                                                                                                                           \
    {                                                                                                                                                                                                                                \
        rocket::Logger::GetGlobolLogger()->pushLog((rocket::LogEvent(rocket::LogLevel::Error).toString()) + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + '\n'); \
    }

#define APPDEBUGLOG(str, ...)                                                                                                                                                                                                           \
    if (rocket::Logger::GetGlobolLogger()->getLoglevel() <= rocket::Debug)                                                                                                                                                              \
    {                                                                                                                                                                                                                                   \
        rocket::Logger::GetGlobolLogger()->pushApplog((rocket::LogEvent(rocket::LogLevel::Debug).toString()) + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + '\n'); \
    }

#define APPINFOLOG(str, ...)                                                                                                                                                                                                           \
    if (rocket::Logger::GetGlobolLogger()->getLoglevel() <= rocket::Info)                                                                                                                                                              \
    {                                                                                                                                                                                                                                  \
        rocket::Logger::GetGlobolLogger()->pushApplog((rocket::LogEvent(rocket::LogLevel::Info).toString()) + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + '\n'); \
    }

#define APPERRORLOG(str, ...)                                                                                                                                                                                                           \
    if (rocket::Logger::GetGlobolLogger()->getLoglevel() <= rocket::Error)                                                                                                                                                              \
    {                                                                                                                                                                                                                                   \
        rocket::Logger::GetGlobolLogger()->pushApplog((rocket::LogEvent(rocket::LogLevel::Error).toString()) + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + '\n'); \
    }

namespace rocket
{
    // std::mutex log_mtx;
    //  格式化字符串(可变参数实现)  printf("%d %s","hello",3);
    template <typename... Args>
    std::string formatString(const char *str, Args &&...args)
    {
        size_t size = snprintf(nullptr, 0, str, args...); // 获取格式化长度后的大小

        std::string result;
        if (size > 0)
        {
            result.resize(size);
            snprintf(&result[0], size + 1, str, args...); // 将格式化字符串输出到string中
        }
        return result;
    }

    // 日志等级
    enum LogLevel
    {
        NONE = 0,
        Debug = 1,
        Info = 2,
        Error = 3
    };
    // 日志级别转字符串
    std::string LogLevelToString(LogLevel level);
    // 字符串转日志级别
    LogLevel StringToLevel(const std::string);

    // 异步日志
    class AsynLogger
    {
    public:
        typedef std::shared_ptr<AsynLogger> s_ptr;
        AsynLogger(const std::string &file_name, const std::string &file_path, int max_size);

        static void loop(AsynLogger *);

        void stop();

        // 刷新到磁盘
        void flush();

        void pushLogBuffer(std::vector<std::string> &vec);

    private:
        std::queue<std::vector<std::string>> m_buffer;
        // m_file_path/m_file_name_yyyymmdd.1
        std::string m_file_name;
        std::string m_file_path;

        int m_max_file_size{0}; // 单个文件最大大小,单位为字节

        std::condition_variable m_sempahore;
        std::condition_variable m_condition;

        std::string m_date;            // 当前打印人日志的文件日期
        FILE *m_file_handler{nullptr}; // 当前打开的日志文件句柄
        bool m_reopen_flag{false};     // 是否需要重新打开新的日志文件

        int m_no{0}; // 日志文件序号

        bool m_stop_flag{false};
    };

    // 日志器(单例模式)
    class Logger
    {
    public:
        typedef std::shared_ptr<Logger> s_ptr;

        Logger(LogLevel level);

        void pushLog(const std::string &msg); // 提交日志入队

        void pushApplog(const std::string &msg);

        LogLevel getLoglevel() const
        {
            return m_set_level;
        }

        void log(); // 打印日志

        void syncLoop();

        void init();

    public:
        // 获取日志器  单例模式
        static Logger *GetGlobolLogger()
        {
            // 锁加双重判断
            if (g_logger == nullptr)
            {
                LogLevel global_log_level = StringToLevel(Config::GetGlobalConfig()->m_log_level); // 获取配置等级
                std::lock_guard<std::mutex> lock(mtx);
                if (g_logger == nullptr)
                {
                    // std::cout << "日志最低等级" << Config::GetGlobalConfig()->m_log_level << std::endl;
                    g_logger = new Logger(global_log_level);
                    g_logger->init();
                }
            }

            return g_logger;
        }

    private: // 单例
        Logger() = default;
        Logger(const Logger &) = delete;
        Logger &operator=(const Logger &) = delete;

    private:
        static Logger *g_logger;
        LogLevel m_set_level;              // 最低打印日志级别
        std::vector<std::string> m_logQue; // 日志消息队列
        std::vector<std::string> m_app_buffer;
        TimerEvent::s_ptr m_timer_event; // 定时器

        AsynLogger::s_ptr m_asnyc_app_logger; // 异步日志
        AsynLogger::s_ptr m_asnyc_logger;
    };

    // 日志
    class LogEvent
    {
    public:
        LogEvent(LogLevel level) : m_level(level)
        {
        }

        // 获取文件名
        std::string getFileName() const
        {
            return m_file_name;
        }
        // 获取日志级别
        LogLevel getLogLevel() const
        {
            return m_level;
        }

        std::string toString();

    private:
        std::string m_file_name; // 文件名
        std::string m_file_line; // 行号
        int m_pid;               // 进程号
        int m_thread_id;         // 线程号

        LogLevel m_level; // 日志级别
    };
}