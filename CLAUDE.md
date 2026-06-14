# 智慧停车管理系统 - 服务器端

## 项目结构
- `ParkSystem_FrontServer/` — 前置服务器（网络I/O网关）
- `ParkSystem_BackServer/`  — 后置服务器（业务逻辑+数据库）

## 架构
客户端 ←TCP→ 前置(Epoll+线程池) ←共享内存+信号量+消息队列→ 后置(线程池+MySQL)

- 前置：C++ Epoll边缘触发，管理客户端连接、心跳(300s超时)、CRC校验、转发
- 后置：C++ 工厂+单例，处理登录/注册/车辆出入场/视频/图片分片上传，对接MySQL
- 通信协议：自定义HEAD(类型+长度+CRC32) + 结构体包体，定义在 protocol.h (两端一致)
- 进程间通信：System V 共享内存(环形缓冲区100块) + 信号量(每块独立锁) + 消息队列(通知索引)

## 构建
- Visual Studio .sln 项目，目标平台 Linux
- 依赖：MySQL Connector/C++、pthread

## 关键文件
- `protocol.h` — 通信协议定义（两服务器一致）
- `CEpollServer.cpp` — 前置epoll主循环
- `CDataTask.cpp` — 后置业务路由分发
- `WriteMemShare.cpp` / `ReadMemShare.cpp` — 共享内存读写
