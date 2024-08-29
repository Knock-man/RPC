#pragma once
#include "rocket/net/coder/abstract_protocol.h"
namespace rocket
{
    struct TinyPBProtocol : public AbstractProtocol
    {
    public:
        TinyPBProtocol() {};
        ~TinyPBProtocol() {};

    public:
        typedef std::shared_ptr<TinyPBProtocol> s_ptr;
        static char PB_START; // 开始符 0x02   1B
        static char PB_END;   // 结束符 0x03   1B

    public:
        int32_t m_pk_len{0};     // 整包长度  4B
        int32_t m_req_id_len{0}; // 消息ID长度 4B

        // req_id 继承父类　／／消息ID

        int32_t m_method_name_len{0}; // 方法名长度 4B
        std::string m_method_name;    // 方法名
        int32_t m_err_code{0};        // 错误码 4B
        int32_t m_err_info_len{0};    // 错误信息长度 4B
        std::string m_err_info;       // 错误信息
        std::string m_pd_data;        // 序列化数据
        int32_t m_check_sum{0};       // 校验和 4B

        bool parase_success{false};
    };

} // namespace rocket
