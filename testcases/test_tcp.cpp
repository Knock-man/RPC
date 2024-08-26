#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fstream>
#include <string.h>
#include <memory>
#include <unistd.h>
#include <filesystem>
#include "../rocket/common/log.h"
#include "../rocket/common/config.h"
#include "../rocket/net/eventloop.h"
#include "../rocket/net/timer_event.h"
#include "../rocket/net/io_thread.h"
#include "../rocket/net/io_thread_group.h"
#include "../rocket/net/tcp/net_addr.h"

int main()
{
    rocket::Config::SetGlobalConfig("rocket.xml");

    rocket::IPNetAddr addr("127.0.0.1", 12345);

    std::cout << addr.toString();

    return 0;
}