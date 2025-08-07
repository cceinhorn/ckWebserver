# RPC
Linux 下 C++ 实现高性能Web服务器, webbench 压力测试 QPS 近万

# 功能
* 基于 one loop per thread 模型, 实现高性能主从 Reactor 网络架构;
* 使用 epoll 边沿触发和非阻塞 I/O 实现多路复用, 提高并发性能;
* 构建基于状态机的 HTTP 请求解析与响应机制，支持多种请求方法(如 GET、POST);
* 支持缓冲区自动扩展, 并通过双缓冲区机制实现高效的异步日志系统;
* 集成内存池管理与 LFU 缓存策略, 提升内存利用效率与系统响应速度;

# 运行环境
* 本地   window10 cpu: i7-11700 内存: 32G
* 虚拟机 CentOS7 4核 8G  
* gcc 9.3.1
* cmake 4.1.0

## 目录树
```
Webserver
├── include         头文件
├── src             源代码
│   ├── http        
│   ├── log         
│   ├── memorypool  
│   └─...
├── test            启动
├── webbench        压力测试  
├── build.sh        编译文件
├── CMakeLists.txt 
├── README.md
└── rpcprovider.png 压测结果
```

# 项目启动
```bash
// 编译
bash build.sh
```
```shell
cd bin
// 启动
./ckserver
```

# 测试结果
虚拟机  CentOS: 7  4核8G

![testwebbench](https://github.com/cceinhorn/Webserver/blob/main/testwebbench.png)

# 致谢
[muduo](https://github.com/chenshuo/muduo)

[webserver](https://github.com/markparticle/WebServer)  
