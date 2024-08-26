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
        int m_size{0};
        std::vector<IOThread *> m_io_thread_groups;

        int m_index{0};
    };

} // namespace rocket
