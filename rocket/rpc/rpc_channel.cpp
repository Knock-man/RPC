#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include "rocket/rpc/rpc_channel.h"
#include "rocket/net/coder/tinypb_protocol.h"
#include "rocket/common/msg_id_util.h"
#include "rocket/rpc/rpc_controller.h"
#include "rocket/common/log.h"
#include "rocket/common/error_code.h"
#include "rocket/net/tcp/tcp_client.h"
#include "rpc_channel.h"
namespace rocket
{
    RpcChannel::RpcChannel(NetAddr::s_ptr peer_addr) : m_peer_addr(peer_addr)
    {
        m_client = std::make_shared<TcpClient>(m_peer_addr);
    }

    RpcChannel::~RpcChannel()
    {
        DEBUGLOG("~RpcChannel");
    }

    void RpcChannel::Init(controller_s_ptr controller, message_s_ptr req, message_s_ptr res, closure_s_ptr done)
    {
        if (m_is_init)
        {
            return;
        }
        m_controller = controller;
        m_request = req;
        m_response = res;
        m_closure = done;
        m_is_init = true;
    }

    void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                                google::protobuf::RpcController *controller, const google::protobuf::Message *request,
                                google::protobuf::Message *response, google::protobuf::Closure *done)
    {

        // 生成msg_id
        TinyPBProtocol::s_ptr req_protocol = std::make_shared<TinyPBProtocol>();  // 发送协议
        RpcController *my_controller = dynamic_cast<RpcController *>(controller); // 控制器
        if (my_controller == NULL)
        {
            ERRORLOG("failed callmethod, RpcController convert error");
            return;
        }

        if (my_controller->GetMsgId().empty()) // 控制器msg_id为空
        {
            req_protocol->m_msg_id = rocket::MsgIdUtil::GenMsgID();
            my_controller->SetMsgId(req_protocol->m_msg_id);
        }
        else
        {
            req_protocol->m_msg_id = my_controller->GetMsgId();
        }

        req_protocol->m_method_name = method->full_name(); // 方法名
        INFOLOG("%s | Call method name[%s]", req_protocol->m_msg_id.c_str(), req_protocol->m_method_name.c_str());

        if (!m_is_init) // 保证对象都被智能指针保存，防止提前析构
        {
            std::string err_info = "RpcChannel not init";
            my_controller->SetError(ERROR_FAILED_SERIALIZE, err_info);
            ERRORLOG("%s | %s,RpcChannel not init", req_protocol->m_msg_id.c_str(), err_info.c_str());
            return;
        }

        // request 序列化 存入m_pd_data
        if (!request->SerializeToString(&(req_protocol->m_pd_data)))
        {
            // 序列化出错
            std::string err_info = "failed to serialize";
            my_controller->SetError(ERROR_FAILED_SERIALIZE, err_info);
            ERRORLOG("%s | %s,origin request [%s]", req_protocol->m_msg_id.c_str(), err_info.c_str(), request->ShortDebugString().c_str());
            return;
        }

        s_ptr channel = shared_from_this(); // 获取当前对象的智能指针

        // 连接服务器
        channel->getTcpClient()->connect([req_protocol, channel]() mutable
                                         {
             RpcController *my_controller = dynamic_cast<RpcController *>(channel->getController()); // 控制器
                        if(channel->getTcpClient()->getConnectErrorCode()!=0)//连接失败
                        {
                            my_controller->SetError(channel->getTcpClient()->getConnectErrorCode(), channel->getTcpClient()->getConnectErrorInfo());
                            ERRORLOG("%s | connect error, error code[%d],error info[%s], peer addr[%s]", 
                                     req_protocol->m_msg_id.c_str(), 
                                     my_controller->GetErrorCode(), 
                                     channel->getTcpClient()->getConnectErrorInfo().c_str(),
                                     channel->getTcpClient()->getPeerAddr()->toString().c_str());
                            return;
                        }
                        //连接成功
                        //发送请求 
                        channel->getTcpClient()->writeMessage(req_protocol, [req_protocol,my_controller, channel](rocket::AbstractProtocol::s_ptr msg_ptr) mutable
                                             { 
                                                INFOLOG("%s|,send request success, call method name[%s], peer addr[%s], local addr[%s]",req_protocol->m_msg_id.c_str(),req_protocol->m_method_name.c_str(),channel->getTcpClient()->getPeerAddr()->toString().c_str(),channel->getTcpClient()->getlocalAddr()->toString().c_str());
                                                //接收响应
                                                channel->getTcpClient()->readMessage(req_protocol->m_msg_id, [my_controller,channel](rocket::AbstractProtocol::s_ptr msg_ptr)mutable
                                                                   {
                                                        //解析响应
                                                         std::shared_ptr<rocket::TinyPBProtocol> rsp_protocol = std::dynamic_pointer_cast<rocket::TinyPBProtocol>(msg_ptr);
                                              INFOLOG("%s | success get response,call method name [%s], peer addr[%s], local addr[%s]", rsp_protocol->m_msg_id.c_str(), rsp_protocol->m_method_name.c_str(),channel->getTcpClient()->getPeerAddr()->toString().c_str(),channel->getTcpClient()->getlocalAddr()->toString().c_str());
                                             
                                              if(!(channel->getResponse()->ParseFromString(rsp_protocol->m_pd_data))){
                                                  ERRORLOG("%s | deserialize error",rsp_protocol->m_msg_id.c_str());
                                                  my_controller->SetError(ERROR_FAILED_SERIALIZE, "serialize error");
                                                  return;
                                              }
                                              if(rsp_protocol->m_err_code!=0)
                                              {
                                                  ERRORLOG("%s | call rpc method[%s] failed,error code[%d],error info[%s]", rsp_protocol->m_msg_id.c_str(), rsp_protocol->m_method_name.c_str(), rsp_protocol->m_err_code, rsp_protocol->m_err_info.c_str());
                                                  my_controller->SetError(rsp_protocol->m_err_code, rsp_protocol->m_err_info);
                                                  return;
                                              }
                                            INFOLOG("%s | call rpc success, call method name[%s], peer addr[%s], local addr[%s]",rsp_protocol->m_msg_id.c_str(), rsp_protocol->m_method_name.c_str(),channel->getTcpClient()->getPeerAddr()->toString().c_str(),channel->getTcpClient()->getlocalAddr()->toString().c_str())
                                              if(channel->getClosure())
                                              channel->getClosure()->Run();//rpc请求完成回调
                                              channel.reset(); }); }); });
    }

    google::protobuf::RpcController *RpcChannel::getController() const
    {
        return m_controller.get();
    }
    google::protobuf::Message *RpcChannel::getRequest() const
    {
        return m_request.get();
    }
    google::protobuf::Message *RpcChannel::getResponse() const
    {
        return m_response.get();
    }
    google::protobuf::Closure *RpcChannel::getClosure() const
    {
        return m_closure.get();
    }
    TcpClient *rocket::RpcChannel::getTcpClient()
    {
        return m_client.get();
    }
}
