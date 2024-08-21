#include "config.h"
#include <tinyxml.h>
#include <mutex>
#include <fstream>
#include "config.h"
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

        m_log_level = log_level_str;
    }

}
