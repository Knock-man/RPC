#ifndef CONFIG_H
#define CONFIG_H

#include <unordered_map>
#include <string>
namespace rocket
{
    class Config
    {
    public:
    public:
        static Config *GetGlobalConfig();
        static void SetGlobalConfig(const char *xmlfile);

    private:
        Config(const char *xmlfile);
        Config(const Config &config) = delete;
        Config &operator=(const Config &config) = delete;

    public:
        static Config *g_config;
        std::string m_log_level; // 日志等级
        std::string m_log_file_name;
        std::string m_log_file_path;
        int m_log_max_file_size{0};
        int m_log_sync_inteval{0}; // 日志同步间隔（毫秒）
    };
}
#endif
