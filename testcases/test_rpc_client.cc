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

void test_tcp_client()
{
    rocket::IPNetAddr::s_ptr addr = std::make_shared<rocket::IPNetAddr>("127.0.0.1", 18010);
    rocket::TcpClient client(addr);
    client.connect([addr, &client]()
                   {
                       DEBUGLOG("conenct to [%s] success", addr->toString().c_str());
                       std::shared_ptr<rocket::TinyPBProtocol> message = std::make_shared<rocket::TinyPBProtocol>();
                       message->m_req_id = "123456789";
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
int main()
{
    rocket::Config::SetGlobalConfig("rocket.xml");

    test_tcp_client();
    return 0;
}