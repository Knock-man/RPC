#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include "rocket/rpc/rpc_channel.h"
#include "rocket/net/coder/tinypb_protocol.h"
#include "rocket/common/msg_id_util.h"
#include "rocket/rpc/rpc_controller.h"
#include "rocket/common/log.h"
#include "rocket/common/error_code.h"
#include "rocket/net/tcp/tcp_client.h"
namespace rocket
{
    RpcChannel::RpcChannel(NetAddr::s_ptr peer_addr) : m_peer_addr(peer_addr)
    {
    }

    RpcChannel::~RpcChannel()
    {
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

        // request 序列化 存入m_pd_data
        if (!request->SerializeToString(&(req_protocol->m_pd_data)))
        {
            // 序列化出错
            std::string err_info = "failed to serialize";
            my_controller->SetError(ERROR_FAILED_SERIALIZE, err_info);
            ERRORLOG("%s | %s,origin request [%s]", req_protocol->m_msg_id.c_str(), err_info.c_str(), request->ShortDebugString().c_str());
            return;
        }

        // 连接服务器
        TcpClient client(m_peer_addr);
        client.connect([&client, req_protocol, done]()
                       {
                        //发送请求 
                        client.writeMessage(req_protocol, [&client, req_protocol, done](rocket::AbstractProtocol::s_ptr msg_ptr)
                                             { 
                                                INFOLOG("%s|,send request success, call method name[%s]",req_protocol->m_msg_id.c_str(),req_protocol->m_method_name.c_str());
                                                //接收响应
                                                client.readMessage(req_protocol->m_msg_id, [done](rocket::AbstractProtocol::s_ptr msg_ptr)
                                                                   {
                                                        //解析响应
                                                         std::shared_ptr<rocket::TinyPBProtocol> rsp_protocol = std::dynamic_pointer_cast<rocket::TinyPBProtocol>(msg_ptr);
                                              INFOLOG("%s | success get response,call method name [%s]", rsp_protocol->m_msg_id.c_str(), rsp_protocol->m_method_name.c_str()); 
                                              if(done)done->Run();//rpc请求完成回调
                                               }); }); });
    }
}