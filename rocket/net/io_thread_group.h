// IO线程组(随机获取，每一个IO线程有一个event_loop)
#pragma once
#include <vector>

#include "io_thread.h"
#include "../rocket/common/log.h"
namespace rocket
{
    class IOThreadGroup
    {
    public:
        IOThreadGroup(int size);

        ~IOThreadGroup();

        void start();

        void join();

        IOThread *getIOThread();

    private:
        int m_size{0};                              // IO线程组大小
        std::vector<IOThread *> m_io_thread_groups; // IO线程组容器

        int m_index{0};
    };

} // namespace rocket
