#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include "rocket/rpc/rpc_dispatcher.h"
#include "rocket/net/coder/tinypb_coder.h"
#include "rocket/common/log.h"
#include "rpc_dispatcher.h"
#include "rocket/common/error_code.h"
#include "rocket/net/tcp/net_addr.h"
#include "rocket/net/tcp/tcp_connection.h"

namespace rocket
{
    void RpcDispatcher::dispatch(AbstractProtocol::s_ptr request, AbstractProtocol::s_ptr response, TcpConection *connection)
    {
        // 强制类型转换
        std::shared_ptr<TinyPBProtocol> req_protocol = std::dynamic_pointer_cast<TinyPBProtocol>(request);
        std::shared_ptr<TinyPBProtocol> rsp_protocol = std::dynamic_pointer_cast<TinyPBProtocol>(response);

        // 获得 协议方法名 = 服务名.方法名
        std::string method_full_name = req_protocol->m_method_name; // 获取协议 m_method_name=服务名.方法名
        std::string service_name;                                   // 服务名
        std::string method_name;                                    // 方法名

        rsp_protocol->m_req_id = rsp_protocol->m_req_id;
        rsp_protocol->m_method_name = rsp_protocol->m_method_name;

        // 解析出服务名 方法名
        if (parse_ServiceFullName(method_full_name, service_name, method_name))
        {
            setTinyPBError(rsp_protocol, ERROR_PARSE_SERVICE_NAME, "parase service name error");
            return;
        }

        // 找到服务名对应的服务对象
        auto it = m_service_map.find(service_name); //<服务名，服务智能指针>
        if (it == m_service_map.end())
        {
            ERRORLOG("%s | service name[%s] not found", rsp_protocol->m_req_id.c_str(), service_name.c_str());
            setTinyPBError(rsp_protocol, ERROR_SERVICE_NIT_FOUND, "service not found");
            return;
        }
        service_s_ptr service = (*it).second; // 具体服务

        // 找到方法名对应的方法
        const google::protobuf::MethodDescriptor *method = service->GetDescriptor()->FindMethodByName(method_name);
        if (method == NULL) // 没有找到方法
        {
            ERRORLOG("%s | method name[%s] not found in service[%s]", rsp_protocol->m_req_id.c_str(), method_name.c_str(), service_name.c_str());
            setTinyPBError(rsp_protocol, ERROR_SERVICE_NIT_FOUND, "method not found");
            return;
        }

        // 根据服务+方法创建 request Message对象对象
        google::protobuf::Message *req_msg = service->GetRequestPrototype(method).New();

        // 反序列化 将pd_data反序列化为 request 对象
        if (req_msg->ParseFromString(req_protocol->m_pd_data))
        {
            ERRORLOG("%s |deserlize error", rsp_protocol->m_req_id.c_str(), method_name.c_str(), service_name.c_str());
            setTinyPBError(rsp_protocol, ERROR_FAILED_DESERIALIZE, "deserlize error");
            if (req_msg != NULL)
            {
                delete req_msg;
                req_msg = NULL;
            }
            return;
        }
        INFOLOG("%s | get rpc request[%s]", req_protocol->m_req_id.c_str(), req_msg->ShortDebugString().c_str());

        // 声明一个response Message对象
        google::protobuf::Message *rsp_msg = service->GetResponsePrototype(method).New();

        // 控制器作用：当调用业务方法的时候，能够通过controller对象获取rpc上的一些信息，必须本地地址，远程地址
        RpcController rpcController; // RPC控制器
        rpcController.SetLocalAddr(connection->getLocalAddr());
        rpcController.SetPeerAddr(connection->getPeerAddr());
        rpcController.SetReqId(req_protocol->m_req_id);
        service->CallMethod(method, &rpcController, req_msg, rsp_msg, NULL); // RPC方法

        // 序列化为字节流
        if (rsp_msg->SerializeToString(&(rsp_protocol->m_pd_data)))
        {
            ERRORLOG("%s |serlize error,origin message", rsp_protocol->m_req_id.c_str(), rsp_msg->ShortDebugString().c_str());
            setTinyPBError(rsp_protocol, ERROR_SERVICE_NIT_FOUND, "serilize error");
            if (req_msg != NULL)
            {
                delete req_msg;
                req_msg = NULL;
            }
            return;
        }

        rsp_protocol->m_err_code = 0; // 设置错误码
        INFOLOG("%s | dispatch sucess,requesut[%s],reponse[%s]", req_protocol->m_req_id.c_str(), req_msg->ShortDebugString().c_str(), rsp_msg->ShortDebugString().c_str());
        delete req_msg;
        delete rsp_msg;
        req_msg = nullptr;
        rsp_msg = nullptr;
    }

    // full_name = 服务名.方法名  解析  service_name=服务名  method_name=方法名
    bool RpcDispatcher::parse_ServiceFullName(const std::string &full_name, std::string &service_name, std::string &method_name)
    {
        if (full_name.empty())
        {
            ERRORLOG("full name empty()");
            return false;
        }
        size_t i = full_name.find_first_not_of(".");
        if (i == full_name.npos)
        {
            ERRORLOG("not find, is full name [%s]", full_name.c_str());
            return false;
        }
        service_name = full_name.substr(0, i);                             // 服务名
        method_name = full_name.substr(i + 1, full_name.length() - i - 1); // 方法名

        INFOLOG("parse service_name[%s] and method_name[%s] from full name [%s]", service_name.c_str(), method_name.c_str(), full_name.c_str());
        return true;
    }

    void RpcDispatcher::registerService(service_s_ptr service)
    {
        std::string service_name = service->GetDescriptor()->full_name();
        m_service_map[service_name] = service;
    }

    void rocket::RpcDispatcher::setTinyPBError(std::shared_ptr<TinyPBProtocol> msg, int32_t err_code, const std::string err_info)
    {
        msg->m_err_code = err_code;
        msg->m_err_info = err_info;
        msg->m_err_info_len = err_info.length();
    }
}
