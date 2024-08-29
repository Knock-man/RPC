#pragma once
#include "rocket/net/coder/abstract_coder.h"
#include "rocket/net/coder/tinypb_protocol.h"
namespace rocket
{
    class TinyPBCoder : public AbstractCoder
    {
    public:
        ~TinyPBCoder() {};
        TinyPBCoder() {};
        // 将message对象转化为字节流写入到out_buffer
        void encode(std::vector<AbstractProtocol::s_ptr> &messages, TcpBuffer::s_ptr out_buffer);

        // 将buffer里面的字节流转化为message对象
        void decode(std::vector<AbstractProtocol::s_ptr> &out_messages, TcpBuffer::s_ptr buffer);

    private:
        const char *encodeTinyPB(TinyPBProtocol::s_ptr message, int &len);
    };

} // namespace rocket
