#include <thread>

#include "io_thread.h"
#include "../rocket/common/log.h"
#include "../rocket/common/util.h"
namespace rocket
{
    IOThread::IOThread()
    {

        pthread_create(&m_thread, NULL, &IOThread::Main, this);

        // wait，直到Main线程初始化完毕
        {
            std::unique_lock<std::mutex> lock(mtx);
            m_init_semaphore.wait(lock);
        }

        DEBUGLOG("IOThread [%d] create success", m_thread_id);
    }
    IOThread::~IOThread()
    {
        m_event_loop->stop();
        pthread_join(m_thread, NULL);

        if (m_event_loop)
        {
            delete m_event_loop;
            m_event_loop = NULL;
        }
    }

    void *IOThread::Main(void *arg)
    {
        IOThread *thread = static_cast<IOThread *>(arg);
        thread->m_event_loop = new EventLoop();
        thread->m_thread_id = getThreadId();

        thread->m_init_semaphore.notify_one();
        DEBUGLOG("IOThread %d created, wait start semaphre", thread->m_thread_id);
        {
            std::unique_lock<std::mutex> lock(mtx);
            thread->m_start_semaphore.wait(lock);
        }
        DEBUGLOG("IOThread %d start loop", thread->m_thread_id);

        thread->m_event_loop->loop();

        DEBUGLOG("IOThread %d end loop", thread->m_thread_id);

        return nullptr;
    }
    void IOThread::start()
    {
        DEBUGLOG("Now invoke IOThread %d", m_thread_id);
        m_start_semaphore.notify_one();
    }

    void IOThread::join()
    {
        pthread_join(m_thread, NULL);
    }
}