# RPC
高性能 RPC 框架

### 日志模块开发
日志模块：  
```
1.日志级别  
2.打印到文件，支持日期命名，以及日志的滚动  
3.C格式化风格
4.线程安全
```  
LogLevel:  
```
Debug
Info
Error
```

LogEvent
```
文件名、行号  
MsgNo  
进程号  
Thread id
日期、以及时间，精确到毫秒  
自定义消息    
```  

日志格式  
```
[Level][%y-%m-%d %H:%M:%s.%ms]\t[pid:thread_id]\t[file_name:line][%msg]
```  

Logger 日志器    
1.提供打印日志的方法    
2.设置日志输出的路径  

### Reator
Reator，又可以称为EventLoop,它本质是一个事件循环模型  

Reactor，它的核心逻辑是一个loop循环，使用伪代码描述如下：  
```C++{.line-number}  
void loop()
{
    while(!stop)
    {
        //执行任务队列的任务
        foreach(task in tasks)
        {
            tack();
        }

        //epoll_wait()

        //1.取得下次定时器任务的时间，与当前设定time_out取较大值，即若下次定时任务时间超过1s就取下次定时任务时间为超时时间，否则取1s
        int time_out = Max(1000,getNextTimeCallback());
        //2.调用Eepoll等待事件发生，超时时间为上述的time_out
        int rt = epoll_wait(efpd,fds,...,time_out);
        if(rt < 0)
        {
            //epoll调用失败
        }else
        {
            if(rt > 0)
            {
                foreach(fd in fds)
                {
                    //添加待执行任务到执行队列
                    tack.push(fd);
                }
            }
        }

        
    }
}

```
### TimeEvent 定时任务  
1.指定时间点 arrive_time  
2.时间间隔interval，单位ms  
3.is_repeated 周期性  
4.is_cancled //取消标志  
5.task  

cancel() //取消定时器  
cancleRepeated() //取消重复  

### Timer  
定时器，他是一个TimeEvent 的集合  
Timer 继承 FdEvent
```
addTimerEvent();  添加定时任务
deleteTimerEvet(); //删除定时任务

onTimer(); //当发生了 IO 事件之和，需要执行的方法

reserArriveTime()

mutimap 存储 TimerEvent <key(arrivetime),TimerEvent>
```

