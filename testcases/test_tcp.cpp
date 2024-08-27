#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fstream>
#include <string.h>
#include <memory>
#include <unistd.h>
#include <filesystem>
#include "rocket/common/log.h"
#include "rocket/common/config.h"
#include "rocket/net/eventloop.h"
#include "rocket/net/timer_event.h"
#include "rocket/net/io_thread.h"
#include "rocket/net/io_thread_group.h"
#include "rocket/net/tcp/net_addr.h"
#include "rocket/net/tcp/tcp_server.h"
void test_tcp_server()
{
    rocket::IPNetAddr::s_ptr addr = std::make_shared<rocket::IPNetAddr>("127.0.0.1", 12738);
    DEBUGLOG("create addr %s", addr->toString().c_str());

    rocket::TcpServer tcp_server(addr);
    tcp_server.start();
}
int main()
{
    rocket::Config::SetGlobalConfig("rocket.xml");
    test_tcp_server();
    return 0;
}