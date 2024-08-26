#pragma once

#include "fd_event.h"
#include <vector>
#include "../../rocket/common/util.h"
namespace rocket
{
    class FdEventGroup
    {
    public:
        FdEventGroup(int size);
        ~FdEventGroup();

        FdEvent *getFdEvent(int fd);

    public:
        static FdEventGroup *getFdEventGroup();

    private:
        int m_size{0};
        std::vector<FdEvent *> m_fd_group;
    };
} // namespace rocket
