#include "io_thread_group.h"
namespace rocket
{
    // 创建size个IO线程(每个线程有一个event_loop)
    IOThreadGroup::IOThreadGroup(int size) : m_size(size)
    {
        m_io_thread_groups.resize(size);
        for (int i = 0; i < size; i++)
        {
            m_io_thread_groups[i] = new IOThread();
        }
    }

    IOThreadGroup::~IOThreadGroup()
    {
    }

    // 启动所有IO线程的loop循环
    void IOThreadGroup::start()
    {
        for (size_t i = 0; i < m_io_thread_groups.size(); i++)
        {
            m_io_thread_groups[i]->start();
        }
    }

    // join()所有IO线程
    void IOThreadGroup::join()
    {
        for (size_t i = 0; i < m_io_thread_groups.size(); i++)
        {
            m_io_thread_groups[i]->join();
        }
    }

    // 随机获取一个IO线程
    IOThread *IOThreadGroup::getIOThread()
    {
        if (m_index == m_io_thread_groups.size() || m_index == -1)
        {
            m_index = 0;
        }
        return m_io_thread_groups[m_index++];
    }
}