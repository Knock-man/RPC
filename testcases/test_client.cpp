// #include <iostream>
// #include <thread>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <fstream>
// #include <string.h>
// #include <memory>
// #include <unistd.h>
// #include <filesystem>
// #include "../rocket/common/log.h"
// #include "../rocket/common/config.h"
// #include "../rocket/net/eventloop.h"
// #include "../rocket/net/timer_event.h"
// #include "../rocket/net/io_thread.h"
// #include "../rocket/net/io_thread_group.h"
// #include "../rocket/net/tcp/net_addr.h"
// #include "../rocket/net/tcp/tcp_server.h"

// void test_connect()
// {
//     // 调用connect连接server
//     // write 一个字符串
//     // 等待read返回结果
//     int fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (fd < 0)
//     {
//         ERRORLOG("invalid fd %d", fd);
//         exit(0);
//     }
//     sockaddr_in server_addr;
//     memset(&server_addr, 0, sizeof(server_addr));
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_port = htons(12345);
//     inet_aton("127.0.0.1", &server_addr.sin_addr);
//     int rt = connect(fd, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr));
//     std::string msg = "hello rocket!";
//     rt = write(fd, msg.c_str(), msg.length());
//     DEBUGLOG("success write %d bytes,[%s]", rt, msg.c_str());
//     char buf[100];
//     rt = read(fd, buf, 100);
//     DEBUGLOG("success read %d bytes,[%s]", rt, std::string(buf).c_str());
// }

// int main()
// {
//     rocket::Config::SetGlobalConfig("rocket.xml");
//     test_connect();
//     return 0;
// }