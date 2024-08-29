#pragma once

#include <unistd.h>
#include <mutex>
#include <iostream>
namespace rocket
{
    static std::mutex mtx;
    pid_t getPid();      // 获取进程ID
    pid_t getThreadId(); // 获取线程ID

    int64_t getNowMs(); // 获取当前时间毫秒数

    // 网络字节序转换为主机字节序
    int32_t getInt32FromNetByte(const char *buf);
};
