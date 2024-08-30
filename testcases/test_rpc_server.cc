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

// 服务器业务
class OrderImpl : public Order
{
    virtual void makeOrder(google::protobuf::RpcController *controller,
                           const ::makeOrderRequest *request,
                           ::makeOrderResponse *response,
                           ::google::protobuf::Closure *done)
    {
        if (request->price() < 10)
        {
            response->set_ret_code(-1);
            response->set_res_info("价格不够");
            return;
        }
        else
        {
            response->set_order_id("20230404");
        }
    }
};
void test_tcp_server()
{
    rocket::IPNetAddr::s_ptr addr = std::make_shared<rocket::IPNetAddr>("127.0.0.1", 10010);
    DEBUGLOG("create addr %s", addr->toString().c_str());

    rocket::TcpServer tcp_server(addr);
    tcp_server.start();
}
int main()
{
    rocket::Config::SetGlobalConfig("rocket.xml");

    std::shared_ptr<OrderImpl> service = std::make_shared<OrderImpl>();
    rocket::RpcDispatcher::GetRpcDispatcher()->registerService(service); // 注册服务

    test_tcp_server();
    return 0;
}