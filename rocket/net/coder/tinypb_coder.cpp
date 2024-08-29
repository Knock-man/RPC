#include <vector>
#include <string.h>
#include <arpa/inet.h>

#include "rocket/net/coder/tinypb_coder.h"
#include "rocket/net/coder/tinypb_protocol.h"
#include "rocket/common/log.h"
#include "rocket/common/util.h"
#include "tinypb_coder.h"

namespace rocket
{
    // 将message对象转化为字节流写入到out_buffer
    void TinyPBCoder::encode(std::vector<AbstractProtocol::s_ptr> &messages, TcpBuffer::s_ptr out_buffer)
    {
        for (auto &message : messages)
        {
            TinyPBProtocol::s_ptr msg = std::dynamic_pointer_cast<TinyPBProtocol>(message); // 父类转子类
            int len = 0;
            const char *buf = encodeTinyPB(msg, len); // message对象编码为字节流
            if (buf != nullptr && len != 0)
            {
                out_buffer->writeToBuffer(buf, len);
            }

            if (buf)
            {
                free((void *)buf);
                buf = NULL;
            }
        }
    }
    // 将buffer里面的字节流转化为message对象
    void TinyPBCoder::decode(std::vector<AbstractProtocol::s_ptr> &out_messages, TcpBuffer::s_ptr buffer)
    {
        while (1)
        {

            // 遍历buffer，找到PB_START,找到之后解析出整包的长度，如何得到结束符位置，判断是否为PB_END
            std::vector<char> tmp = buffer->m_buffer;
            int start_index = buffer->readIndex(); // 包的起始索引
            int end_index = -1;                    // 包的结束索引

            int pk_len = 0;            // 整包长度
            bool parse_sucess = false; // 标记是否读取成功
            int i;
            for (i = start_index; i < buffer->writeIndex(); i++)
            {
                if (tmp[i] == TinyPBProtocol::PB_START)
                {
                    // 从下取4个字节 由于是网络字节序，需要转化为主机字节序
                    if (i + 1 < buffer->writeIndex())
                    {
                        pk_len = getInt32FromNetByte(&tmp[i + 1]);
                        DEBUGLOG("get pk_len = %d", pk_len);

                        // 结束符的索引
                        int j = i + pk_len - 1;
                        if (j >= buffer->writeIndex()) // 判断结束符位置是否超出
                            continue;
                        if (tmp[j] = TinyPBProtocol::PB_END) // 找到结束符
                        {
                            start_index = i; // 更新下一次遍历包的起点
                            end_index = j;
                            parse_sucess = true; // 标记读取成功
                            break;
                        }
                    }
                }
            }
            if (i >= buffer->writeIndex())
            {
                DEBUGLOG("decode end,read all buffer data");
                return;
            }
            // 解析成功
            if (parse_sucess)
            {
                buffer->moveReadIndex(end_index - start_index + 1); // 缓冲区向后移动可读索引
                TinyPBProtocol::s_ptr message = std::make_shared<TinyPBProtocol>();
                message->m_pk_len = pk_len; // 包长

                int msg_id_len_index = start_index + sizeof(char) + sizeof(message->m_pk_len); // 消息ID长度
                if (msg_id_len_index >= end_index)
                {
                    message->parase_success = false;
                    ERRORLOG("parase error, msg_id_len_index[%d] > end_index[%d]", msg_id_len_index, end_index);
                    continue;
                }
                message->m_msg_id_len = getInt32FromNetByte(&tmp[msg_id_len_index]); // 消息ID长度
                DEBUGLOG("parase msg_id_len=%d", message->m_msg_id_len);

                int msg_id_index = msg_id_len_index + sizeof(message->m_msg_id_len);
                char msg_id[100] = {0};
                memcpy(&msg_id[0], &tmp[msg_id_index], message->m_msg_id_len);
                message->m_msg_id = std::string(msg_id); // 消息ID
                DEBUGLOG("parase msg_id=%s", message->m_msg_id.c_str());

                int method_name_len_index = msg_id_index + message->m_msg_id_len;
                if (method_name_len_index >= end_index)
                {
                    message->parase_success = false;
                    ERRORLOG("parase error, method_name_len_index[%d] > end_index[%d]", method_name_len_index, end_index);
                    continue;
                }
                message->m_method_name_len = getInt32FromNetByte(&tmp[method_name_len_index]); // 方法名长度

                int method_name_index = method_name_len_index + sizeof(message->m_method_name_len);
                char method_name[512] = {0};
                memcpy(&method_name[0], &tmp[method_name_index], message->m_method_name_len);
                message->m_method_name = std::string(method_name); // 消息ID
                DEBUGLOG("parase m_method_name=%s", message->m_method_name.c_str());

                int err_code_index = method_name_index + message->m_method_name_len;
                if (err_code_index >= end_index)
                {
                    message->parase_success = false;
                    ERRORLOG("parase error, err_code_index[%d] > end_index[%d]", err_code_index, end_index);
                    continue;
                }
                message->m_err_code = getInt32FromNetByte(&tmp[err_code_index]); // 错误码

                int error_info_len_index = err_code_index + sizeof(message->m_err_code);
                if (error_info_len_index >= end_index)
                {
                    message->parase_success = false;
                    ERRORLOG("parase error, error_info_len_index[%d] > end_index[%d]", error_info_len_index, end_index);
                    continue;
                }
                message->m_err_info_len = getInt32FromNetByte(&tmp[error_info_len_index]); // 错误信息长度

                int err_info_index = error_info_len_index + sizeof(message->m_err_info_len);
                char error_info[512] = {0};
                memcpy(&error_info[0], &tmp[err_info_index], message->m_err_info_len);
                message->m_err_info = std::string(error_info); // 错误消息
                DEBUGLOG("parase m_err_info=%s", message->m_err_info.c_str());

                // 数据段长度
                int pd_data_len = message->m_pk_len - message->m_method_name_len - message->m_msg_id_len - message->m_err_info_len - 24 - 2;
                int pd_data_index = err_info_index + message->m_err_info_len;
                message->m_pd_data = std::string(&tmp[pd_data_index], pd_data_len); // 数据段

                // 这里校验和去解析
                message->parase_success = true;

                out_messages.push_back(message);
            }
        }
    }

    const char *TinyPBCoder::encodeTinyPB(TinyPBProtocol::s_ptr message, int &len)
    {
        // 消息ID
        if (message->m_msg_id.empty())
        {
            message->m_msg_id = "123456789";
        }
        // getchar();
        //  printf("%s\n", message->m_msg_id.c_str());
        DEBUGLOG("msg_id = %s", message->m_msg_id.c_str());

        // 整包长度
        int pk_len = 2 + 24 + message->m_msg_id.length() + message->m_method_name.length() + message->m_err_info.length() + message->m_pd_data.length();
        DEBUGLOG("pk_len=%d", pk_len);

        char *buf = reinterpret_cast<char *>(malloc(pk_len)); // 申请一段内存
        char *tmp = buf;

        *tmp = TinyPBProtocol::PB_START; // 起始位
        tmp++;

        // 包长度
        int32_t pk_len_net = htonl(pk_len);
        memcpy(tmp, &pk_len_net, sizeof(pk_len_net));
        tmp += sizeof(pk_len_net);

        // 消息ID长度
        int msg_id_len = message->m_msg_id.length();
        int32_t msg_id_len_net = htonl(msg_id_len);
        memcpy(tmp, &msg_id_len_net, sizeof(msg_id_len_net));
        tmp += sizeof(msg_id_len_net);

        // 消息ID
        if (!message->m_msg_id.empty())
        {
            memcpy(tmp, &(message->m_msg_id[0]), msg_id_len);
            tmp += msg_id_len;
        }

        // 方法名长度
        int methon_name_len = message->m_method_name.length();
        int32_t methon_name_len_net = htonl(methon_name_len);
        memcpy(tmp, &methon_name_len_net, sizeof(methon_name_len_net));
        tmp += sizeof(methon_name_len_net);

        // 方法名
        if (!message->m_method_name.empty())
        {
            memcpy(tmp, &(message->m_method_name[0]), methon_name_len);
            tmp += methon_name_len;
        }

        // 错误码
        int32_t err_code_net = htonl(message->m_err_code);
        memcpy(tmp, &err_code_net, sizeof(err_code_net));
        tmp += sizeof(err_code_net);

        // 错误信息长度
        int err_info_len = message->m_err_info.length();
        int32_t err_info_len_net = htonl(err_info_len);
        memcpy(tmp, &err_info_len_net, sizeof(err_info_len_net));
        tmp += sizeof(err_info_len_net);

        // 错误信息
        if (!message->m_err_info.empty())
        {
            memcpy(tmp, &(message->m_err_info[0]), err_info_len);
            tmp += err_info_len;
        }

        // 消息体
        if (!message->m_pd_data.empty())
        {
            memcpy(tmp, &(message->m_pd_data[0]), message->m_pd_data.length());
            tmp += message->m_pd_data.length();
        }

        // 校验和
        int32_t check_sum_net = htonl(1);
        memcpy(tmp, &check_sum_net, sizeof(check_sum_net));
        tmp += sizeof(check_sum_net);

        // 结束符
        *tmp = TinyPBProtocol::PB_END;

        // 防止这些长度信息为空
        message->m_pk_len = pk_len;
        message->m_msg_id_len = msg_id_len;
        message->m_method_name_len = methon_name_len;
        message->m_err_info_len = err_info_len;

        message->parase_success = true;
        len = pk_len; // 出参 解析出来的长度

        DEBUGLOG("encode message[%s] success", message->m_msg_id.c_str());
        return buf;
    }
}
