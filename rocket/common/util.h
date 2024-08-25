#pragma once

#include <unistd.h>
#include <mutex>
#include <iostream>
namespace rocket
{
    static std::mutex mtx;
    pid_t getPid();      // 获取进程ID
    pid_t getThreadId(); // 获取线程ID
};
