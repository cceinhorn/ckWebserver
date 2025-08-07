# RPC
Linux下C++实现高性能Web服务器, webbench压力测试QPS近万

# 功能
* 基于 one loop per thread 模型, 实现高性能主从 Reactor 网络架构;
* 使用 epoll 边沿触发和 非阻塞 I/O 实现多路复用, 提高并发性能;
* 构建基于 状态机 的 HTTP 请求解析与响应机制，支持多种请求方法（如 GET、POST;
* 支持 缓冲区自动扩展, 并通过双缓冲区机制实现高效的异步日志系统;
* 集成 内存池管理 与 LFU 缓存策略, 提升内存利用效率与系统响应速度;

# 运行环境
* 本地   window10 cpu: i7-11700 内存: 32G
* 虚拟机 CentOS7 4核 8G  
* gcc 9.3.1
* cmake 4.1.0

# 库准备
```shell
// 基础工具
sudo apt-get install -y wget cmake build-essential unzip
```
muduo安装需要依赖boost

[boost](https://www.boost.org/releases/latest/)
```shell
sudo apt-get install -y libboost-all-dev
```
[muduo](https://blog.csdn.net/QIANGWEIYUAN/article/details/89023980) 

[protobuf](https://github.com/google/protobuf)
```shell
sudo apt-get install protobuf-compiler libprotobuf-dev
```

[zookeeper](https://zookeeper.apache.org/releases.html)
```shell
sudo apt install zookeeperd
```

## 目录树
```
.
├── bin             可执行文件
│   └── test.conf   ip:port配置文件
├── include         框架头文件
│   └─...
├── src             框架源代码
│   └─...
├── test           
│   ├── callee      服务提供方
│   ├── caller      服务请求方
│   └── .proto      接口及数据结构描述
├── build.sh        编译文件
├── CMakeLists.txt 
├── README.md
├── rpcconsumer.png 请求测试 
└── rpcprovider.png 注册测试
```

# 项目启动
```bash
// 编译
bash build.sh
```
```shell
cd bin
// 开启对应的zookeeper
// 服务提供方注册服务
./provider -i test.conf
// 服务请求方消费服务
./consumer -i test.conf
```

# 测试结果
![testprovider](https://github.com/cceinhorn/ckRPC/blob/master/rpcprovider.png)

![testconsumer](https://github.com/cceinhorn/ckRPC/blob/master/rpcconsumer.png)

# 致谢
[muduo](https://github.com/chenshuo/muduo)

[webserver](https://github.com/markparticle/WebServer)  
