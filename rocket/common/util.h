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
};
