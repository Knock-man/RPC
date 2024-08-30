#include "config.h"
#include <tinyxml.h>
#include <fstream>
#include "util.h"
#include "iostream"

#define READ_SML_NODE(name, parent)                                            \
    TiXmlElement *name##_node = parent->FirstChildElement(#name);              \
    if (!(name##_node))                                                        \
    {                                                                          \
        printf("Start rocket server error,failed to read node [%s]\n", #name); \
        exit(0);                                                               \
    };

#define READ_STR_FROM_SML_NOOE(name, parent)                                        \
    TiXmlElement *name##_node = parent->FirstChildElement(#name);                   \
    if (!name##_node || !name##_node->GetText())                                    \
    \                   
{                                                            \
        printf("Start rocket server error,failed to read config file %s\n", #name); \
        exit(0);                                                                    \
    }                                                                               \
    std::string name##_str = std::string(name##_node->GetText());

namespace rocket
{
    Config *Config::g_config = nullptr;

    Config *Config::GetGlobalConfig()
    {
        return g_config;
    }
    void Config::SetGlobalConfig(const char *xmlfile)
    {
        if (g_config == nullptr)
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (g_config == nullptr)
            {
                g_config = new Config(xmlfile);
            }
        }
    }

    Config::Config(const char *xmlfile)
    {
        std::ifstream file("rocket.xml");

        TiXmlDocument *xml_document = new TiXmlDocument(xmlfile);

        bool rt = xml_document->LoadFile(xmlfile, TIXML_ENCODING_UTF8);
        if (!rt) // 加载配置文件失败
        {
            printf("Start rocket server error,failed to read config file %s, error info[%s]\n", xmlfile, xml_document->ErrorDesc());
            exit(0);
        }

        READ_SML_NODE(root, xml_document); //<root></root>

        READ_SML_NODE(log, root_node); //<log></log>

        READ_STR_FROM_SML_NOOE(log_level, log_node); //<log_level></log_level>
        READ_STR_FROM_SML_NOOE(log_file_name, log_node);
        READ_STR_FROM_SML_NOOE(log_file_path, log_node);
        READ_STR_FROM_SML_NOOE(log_max_file_size, log_node);
        READ_STR_FROM_SML_NOOE(log_sync_interval, log_node);

        m_log_level = log_level_str;                                    // 日志等级
        m_log_file_name = log_file_name_str;                            // 日志文件名
        m_log_file_path = log_file_path_str;                            // 日志文件路径
        m_log_max_file_size = std::atoi(log_max_file_size_str.c_str()); // 日志文件最大
        m_log_sync_inteval = std::atoi(log_sync_interval_str.c_str());  // 日志同步间隔（毫秒）
    }
}
