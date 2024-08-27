#pragma once
#include <memory>
#include <string>

namespace rocket
{
    class AbstractProtocol : public std::enable_shared_from_this<AbstractProtocol>
    {
    public:
        typedef std::shared_ptr<AbstractProtocol> s_ptr;

        std::string getReqId() const
        {
            return m_req_id;
        }
        void setReqId(const std::string &req_id)
        {
            m_req_id = req_id;
        }
        virtual ~AbstractProtocol() {};

    protected:
        std::string m_req_id; // 请求号，唯一表示一个请求或者响应
    };
}