# tundra-server
a web server based on TCP

features：
* 使用变长缓冲池和mmap后端写入的异步日志库，每秒可记录百万条日志(最高2.4Mlog/s)。
* 借助阻塞队列实现了基于任务的线程池，广泛运用C++11技术
* 完善的Reactor结构，EventLoop作为主事件循环，使用Poller实现IO复用，Channel实现事件分发
* 遵守one loop per thread, 所有的回调函数只能在loop的线程执行，以保证线程安全
* 基于红黑树的定时器Timer设计，采用timerfd接入Reactor结构，无需特殊处理
* 使用弱回调处理事件，防止调用被析构对象的函数；依然使用弱回调定时关闭TCP连接，避免double-free

TODO ：
- [ ] 简易的http服务器组件
- [ ] 简易的grpc实现

