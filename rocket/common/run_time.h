#pragma once
#include <string>
namespace rocket
{
    class RunTime
    {
    public:
        static RunTime *GetRunTime()
        {
            static thread_local RunTime instance;
            return &instance;
        }

    public:
        std::string m_msgid;
        std::string m_method_name;
    };
}