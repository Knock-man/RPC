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
using namespace std;

void test_io_thread()
{
}
int main()
{

    rocket::Config::SetGlobalConfig("rocket.xml");

    test_io_thread();
    rocket::EventLoop *eventloop = new rocket::EventLoop();

    int listedfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listedfd == -1)
    {
        ERRORLOG("listedfd = -1");
        exit(0);
    }

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_port = htons(8787);
    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr.sin_addr);

    int rt = bind(listedfd, (sockaddr *)&addr, sizeof(addr));
    if (rt != 0)
    {
        ERRORLOG("bind error");
        exit(1);
    }

    rt = listen(listedfd, 10);
    if (rt != 0)
    {
        ERRORLOG("listen error");
        exit(1);
    }

    rocket::FdEvent event(listedfd);
    event.listen(rocket::FdEvent::IN_EVENT, [listedfd]() { // 连接事件
        sockaddr_in peer_adr;
        socklen_t addre_len = sizeof(sockaddr);
        memset(&peer_adr, 0, sizeof(peer_adr));
        int clientfd = accept(listedfd, (sockaddr *)&peer_adr, &addre_len);

        inet_ntoa(peer_adr.sin_addr);
        DEBUGLOG("success get client fd[%d] peer addr:[%s:%d]", clientfd, inet_ntoa(peer_adr.sin_addr), ntohs(peer_adr.sin_port));
    });

    int i = 0;
    rocket::TimerEvent::s_ptr timer_event = std::make_shared<rocket::TimerEvent>(
        1000, true, [&i]()
        { INFOLOG("%d 次触发定时事件", i++); });

    rocket::IOThread io_thread;
    io_thread.getEventLoop()->addEpollEvent(&event);      // 添加连接监听
    io_thread.getEventLoop()->addTimerEvent(timer_event); // 添加超时监听
    io_thread.start();
    io_thread.join();

    return 0;
}
