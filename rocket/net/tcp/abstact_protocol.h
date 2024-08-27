#pragma once
#include <memory>
namespace rocket
{
    class AbstractProtocol
    {
    public:
        typedef std::shared_ptr<AbstractProtocol> s_ptr;
    };
}