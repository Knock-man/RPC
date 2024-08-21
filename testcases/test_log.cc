#include <iostream>
#include <thread>
#include <fstream>
#include "../rocket/common/log.h"
#include "../rocket/common/config.h"
using namespace std;

void func()
{
    int i = 100;
    while (i--)
    {
        DEBUGLOG("text debug log in thread %s", "func");
        INFOLOG("text info log in thread %s", "func");
    }
}
int main()
{
    rocket::Config::SetGlobalConfig("rocket.xml");
    thread t(func);

    int i = 20;
    while (i--)
    {
        DEBUGLOG("text debug log in thread %s", "11");
        INFOLOG("text info log in thread %s", "11");
    }
    t.join();
    ERRORLOG("text error log %s", "33")
    return 0;
}
