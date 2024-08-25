#pragma once

#include "fd_event.h"
namespace rocket
{
    class WakeUpFdEvent : public FdEvent
    {
    public:
        WakeUpFdEvent(int fd);
        ~WakeUpFdEvent();

        void wakeup();

    private:
    };
};