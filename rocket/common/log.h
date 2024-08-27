#pragma once
#include <string>
#include <memory>
#include <queue>
#include "config.h"
#include "util.h"
#include <iostream>

#define DEBUGLOG(str, ...)                                                                                                                                                                                                           \
    if (rocket::Logger::GetGlobolLogger()->getLoglevel() <= rocket::Debug)                                                                                                                                                           \
    {                                                                                                                                                                                                                                \
        rocket::Logger::GetGlobolLogger()->pushLog((rocket::LogEvent(rocket::LogLevel::Debug).toString()) + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + '\n'); \
        rocket::Logger::GetGlobolLogger()->log();                                                                                                                                                                                    \
    }

#define INFOLOG(str, ...)                                                                                                                                                                                                           \
    if (rocket::Logger::GetGlobolLogger()->getLoglevel() <= rocket::Info)                                                                                                                                                           \
    {                                                                                                                                                                                                                               \
        rocket::Logger::GetGlobolLogger()->pushLog((rocket::LogEvent(rocket::LogLevel::Info).toString()) + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + '\n'); \
        rocket::Logger::GetGlobolLogger()->log();                                                                                                                                                                                   \
    }

#define ERRORLOG(str, ...)                                                                                                                                                                                                           \
    if (rocket::Logger::GetGlobolLogger()->getLoglevel() <= rocket::Error)                                                                                                                                                           \
    {                                                                                                                                                                                                                                \
        rocket::Logger::GetGlobolLogger()->pushLog((rocket::LogEvent(rocket::LogLevel::Error).toString()) + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + '\n'); \
        rocket::Logger::GetGlobolLogger()->log();                                                                                                                                                                                    \
    }

namespace rocket
{
    // std::mutex log_mtx;
    //  格式化字符串(可变参数实现)  printf("%d %s","hello",3);
    template <typename... Args>
    std::string formatString(const char *str, Args &&...args)
    {
        int size = snprintf(nullptr, 0, str, args...); // 获取格式化长度后的大小

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

    // 日志器(单例模式)
    class Logger
    {
    public:
        typedef std::shared_ptr<Logger> s_ptr;

        Logger(LogLevel level) : m_set_level(level) {};

        void pushLog(const std::string &msg); // 提交日志入队

        LogLevel getLoglevel() const
        {
            return m_set_level;
        }

        void log(); // 打印日志

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
        LogLevel m_set_level;             // 最低打印日志级别
        std::queue<std::string> m_logQue; // 日志消息队列
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