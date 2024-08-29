#pragma once

#include "rocket/net/coder/abstract_coder.h"
#include "rocket/net/coder/abstract_protocol.h"
#include "rocket/net/coder/string_coder.h"

namespace rocket
{
    class StringProtocol : public AbstractProtocol
    {
    public:
        std::string info;
    };

    class StringCoder : public AbstractCoder
    {
    public:
        // 将message对象转化为字节流写入到out_buffer
        void encode(std::vector<AbstractProtocol::s_ptr> &messages, TcpBuffer::s_ptr out_buffer)
        {
            for (int i = 0; i < messages.size(); i++)
            {
                std::shared_ptr<StringProtocol> msg = std::dynamic_pointer_cast<StringProtocol>(messages[i]);
                out_buffer->writeToBuffer(msg->info.c_str(), msg->info.length());
            }
        }
        // 将buffer里面的字节流转化为message对象
        void decode(std::vector<AbstractProtocol::s_ptr> &out_messages, TcpBuffer::s_ptr buffer)
        {
            std::vector<char> re;
            buffer->readFromBuffer(re, buffer->readAble());
            std::string info(re.begin(), re.end());

            std::shared_ptr<StringProtocol> msg = std::make_shared<StringProtocol>();
            msg->info = info;
            msg->m_req_id = "123456";
            out_messages.push_back(msg);
        }
    };
}