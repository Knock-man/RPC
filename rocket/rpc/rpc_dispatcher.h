#pragma once

#include <map>
#include <memory>
#include <google/protobuf/service.h>

#include "rocket/net/coder/abstract_protocol.h"
#include "rocket/net/coder/tinypb_protocol.h"
#include "rocket/rpc/rpc_controller.h"
#include "rocket/net/tcp/tcp_connection.h"

namespace rocket
{

    class TcpConection;

    class RpcDispatcher
    {
    public:
        typedef std::shared_ptr<google::protobuf::Service> service_s_ptr;
        void dispatch(AbstractProtocol::s_ptr request, AbstractProtocol::s_ptr response, TcpConection *connection);

        void registerService(service_s_ptr service);

        void setTinyPBError(std::shared_ptr<TinyPBProtocol> msg, int32_t err_code, const std::string err_info);

    private:
        // full_name = 服务名.方法名  解析  service_name=服务名  method_name=方法名
        bool parse_ServiceFullName(const std::string &full_name, std::string &service_name, std::string &method_name);

    private:
        std::map<std::string, service_s_ptr> m_service_map;
    };
}