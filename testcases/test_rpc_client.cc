#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fstream>
#include <string.h>
#include <memory>
#include <unistd.h>
#include <filesystem>
#include <google/protobuf/service.h>

#include "../rocket/common/log.h"
#include "../rocket/common/config.h"
#include "../rocket/net/eventloop.h"
#include "../rocket/net/timer_event.h"
#include "../rocket/net/io_thread.h"
#include "rocket/net/io_thread_group.h"
#include "../rocket/net/tcp/net_addr.h"
#include "../rocket/net/tcp/tcp_server.h"
#include "../rocket/net/tcp/tcp_client.h"
#include "rocket/net/coder/string_coder.h"
#include "rocket/net/coder/abstract_protocol.h"
#include "rocket/net/coder/tinypb_coder.h"
#include "order.pb.h"
#include "rocket/net/tcp/net_addr.h"
#include "rocket/net/tcp/tcp_server.h"
#include "rocket/rpc/rpc_dispatcher.h"
#include "rocket/rpc/rpc_channel.h"
#include "rocket/rpc/rpc_controller.h"
#include "rocket/rpc/rpc_closeure.h"

#define IP_PORT "127.0.0.1:17311"
void test_tcp_client()
{
    rocket::IPNetAddr::s_ptr addr = std::make_shared<rocket::IPNetAddr>("127.0.0.1", 19110);
    rocket::TcpClient client(addr);
    client.connect([addr, &client]()
                   {
                       DEBUGLOG("conenct to [%s] success", addr->toString().c_str());
                       std::shared_ptr<rocket::TinyPBProtocol> message = std::make_shared<rocket::TinyPBProtocol>();
                       message->m_msg_id = "123456789";
                       message->m_pd_data = "test pd data";

                       makeOrderRequest request; // 请求对象
                       request.set_price(100);
                       request.set_goods("app");
                       if (!request.SerializeToString(&(message->m_pd_data))) // 序列化m_pd_data段
                       {
                           ERRORLOG("serilize error");
                           return;
                       }
                       message->m_method_name = "Order.makeOrder"; // 方法名

                       client.writeMessage(message, [request](rocket::AbstractProtocol::s_ptr msg_ptr)
                                           { 
                                            DEBUGLOG("send message sucess,request[%s]",request.ShortDebugString().c_str()); 
                                            });
                       client.readMessage("123456789", [](rocket::AbstractProtocol::s_ptr msg_ptr)
                                          {
                                              std::shared_ptr<rocket::TinyPBProtocol> message = std::dynamic_pointer_cast<rocket::TinyPBProtocol>(msg_ptr);
                                              //DEBUGLOG("req_id[%s], get response [%s]", message->m_req_id.c_str(), message->m_pd_data.c_str());
                                              makeOrderResponse response;//反序列化接收响应体
                                              if(!response.ParseFromString(message->m_pd_data)){
                                                  ERRORLOG("deserialize error");
                                                  return;
                                              }
                                              DEBUGLOG("get response success, response[%s]", response.ShortDebugString().c_str());
                                              }); });
}

void test_rpc_channel()
{

    // 构建请求对象
    // std::shared_ptr<makeOrderRequest> request = std::make_shared<makeOrderRequest>();
    NEWMESSAGE(makeOrderRequest, request);
    request->set_price(100);
    request->set_goods("app");

    // 响应对象
    // std::shared_ptr<makeOrderResponse> response = std::make_shared<makeOrderResponse>();
    NEWMESSAGE(makeOrderResponse, response);

    // 控制器
    // std::shared_ptr<rocket::RpcController> controller = std::make_shared<rocket::RpcController>();
    NEWRPCCONTROLLER(controller);
    controller->SetTimeout(100000);
    controller->SetMsgId("123456789");

    // channel
    // rocket::IPNetAddr::s_ptr addr = std::make_shared<rocket::IPNetAddr>("127.0.0.1", 18110);
    // std::shared_ptr<rocket::RpcChannel> channel = std::make_shared<rocket::RpcChannel>(addr);
    // NEWCHANNEL("127.0.0.1:18110", channel);
    NEWCHANNEL(IP_PORT, channel);

    // 回调函数
    std::shared_ptr<rocket::RpcClosure> closure = std::make_shared<rocket::RpcClosure>([request, response, channel, controller]() mutable
                                                                                       { 
                                                                                        if(controller->GetErrorCode()==0)
                                                                                        {
                                                                                            INFOLOG("call rpc success,resuest[%s],response[%s]", request->ShortDebugString().c_str(), response->ShortDebugString().c_str());
                                                                                            //业务逻辑
                                                                                        }else{
                                                                                            ERRORLOG("call rpc failed,resuest[%s],error code[%d],error info[%s]", request->ShortDebugString().c_str(), controller->GetErrorCode(),controller->GetErrorInfo().c_str());
                                                                                        }
                                                                                       
                                                                                        INFOLOG("now exit eventloop");
                                                                                        channel->getTcpClient()->stop();
                                                                                        channel.reset(); });

    // channel->Init(controller, request, response, closure);

    // stub.makeOrder 底层调用RpcChannel::CallMethod()方法
    // Order_Stub stub(channel.get());
    // stub.makeOrder(controller.get(), request.get(), response.get(), closure.get());
    CALLRPC(IP_PORT, makeOrder, controller, request, response, closure);
}
int main()
{
    rocket::Config::SetGlobalConfig("rocket.xml");

    test_rpc_channel();
    return 0;
}