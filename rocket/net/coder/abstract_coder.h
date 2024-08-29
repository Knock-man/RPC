#pragma once
#include <vector>
#include "rocket/net/tcp/tcp_buffer.h"
#include "rocket/net/coder/abstract_protocol.h"
namespace rocket
{
    class AbstractCoder
    {
    public:
        // 将message对象转化为字节流写入到out_buffer
        virtual void encode(std::vector<AbstractProtocol::s_ptr> &messages, TcpBuffer::s_ptr out_buffer) = 0;
        // 将buffer里面的字节流转化为message对象
        virtual void decode(std::vector<AbstractProtocol::s_ptr> &out_messages, TcpBuffer::s_ptr buffer) = 0;

        virtual ~AbstractCoder() {};
    };
}