#include "util.h"
#include "sys/syscall.h"

namespace rocket
{
    static int g_pid = 0;
    static thread_local int g_thread_id = 0;

    pid_t getPid()
    {
        if (g_pid == 0)
        {
            // 缓存
            g_pid = getpid();
        }
        return g_pid;
    }

    pid_t getThreadId()
    {
        if (g_thread_id == 0)
        {
            g_thread_id = syscall(SYS_gettid);
        }
        return g_thread_id;
    }
}
