# C++ 房间服务器 v1

基于 C++17 / Boost.Asio / Protobuf 实现的房间制游戏服务器 Demo。

项目目标是跑通一个游戏服务器的核心业务闭环：客户端连接、登录模拟、在线玩家管理、匹配队列、房间创建、房间广播、聊天同步、定时 Tick、断线清理和客户端压测。

---

## 一、项目功能

当前 v1 已实现：

* 异步 TCP 服务器
* 自定义 TCP 消息协议
* TCP 粘包 / 拆包处理
* Protobuf 消息序列化 / 反序列化
* MessageDispatcher 消息分发
* 登录模拟
* 玩家连接管理
* 匹配队列
* 2 人房间创建
* 房间创建通知
* 房间内聊天广播
* 房间定时 Tick
* 客户端断线清理
* 多客户端模拟测试
* 服务端状态统计

---

## 二、技术栈

* C++17
* Boost.Asio
* Protobuf
* CMake
* WSL Ubuntu
* CLion

---

## 三、项目结构

```text
game_server/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── architecture.md
│   └── protocol.md
├── proto/
│   └── message.proto
├── src/
│   ├── main.cpp
│   ├── net/
│   │   ├── Session.h
│   │   ├── Session.cpp
│   │   ├── Message.h
│   │   ├── MessageCodec.h
│   │   ├── MessageCodec.cpp
│   │   ├── MessageDispatcher.h
│   │   └── MessageDispatcher.cpp
│   ├── player/
│   │   ├── PlayerManager.h
│   │   └── PlayerManager.cpp
│   ├── service/
│   │   ├── LoginService.h
│   │   ├── LoginService.cpp
│   │   ├── MatchService.h
│   │   ├── MatchService.cpp
│   │   ├── ChatService.h
│   │   └── ChatService.cpp
│   ├── match/
│   │   ├── MatchQueue.h
│   │   └── MatchQueue.cpp
│   ├── room/
│   │   ├── Room.h
│   │   ├── Room.cpp
│   │   ├── RoomManager.h
│   │   └── RoomManager.cpp
│   └── generated/
│       └── proto/
│           ├── message.pb.h
│           └── message.pb.cc
└── client/
    ├── main.cpp
    ├── TcpClient.h
    ├── TcpClient.cpp
    ├── ClientSimulator.h
    └── ClientSimulator.cpp
```

---

## 四、核心流程

### 1. 登录流程

```text
Client
  -> LoginReq
Server
  -> LoginService
  -> 生成 player_id
  -> PlayerManager 保存 player_id -> Session
  -> LoginResp
```

### 2. 匹配流程

```text
Client
  -> MatchReq
Server
  -> MatchService
  -> MatchQueue::join(player_id)
  -> 队列人数达到 2 人
  -> RoomManager::create_room()
  -> RoomCreatedNotify
```

### 3. 聊天流程

```text
Client
  -> ChatReq
Server
  -> ChatService
  -> RoomManager::get_room_by_player(player_id)
  -> Room::handle_chat()
  -> Room::broadcast(ChatNotify)
```

### 4. Tick 流程

```text
Server Timer
  -> RoomManager::tick_all()
  -> Room::tick()
```

### 5. 断线清理流程

```text
Client disconnect
  -> Session::close()
  -> close_callback
  -> PlayerManager::remove_player()
  -> MatchQueue::leave()
  -> RoomManager::remove_player()
  -> Room 为空时删除房间
```

---

## 五、运行方式

### 1. 安装依赖

```bash
sudo apt update
sudo apt install libboost-all-dev protobuf-compiler libprotobuf-dev
```

检查 Protobuf：

```bash
protoc --version
```

---

### 2. 生成 Protobuf C++ 文件

```bash
protoc --cpp_out=src/generated proto/message.proto
```

如果你的 `message.proto` 放在 `proto/` 下，生成后目录应类似：

```text
src/generated/proto/message.pb.h
src/generated/proto/message.pb.cc
```

---

### 3. 构建项目

```bash
cmake -S . -B cmake-build-debug-wsl -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-debug-wsl
```

---

### 4. 启动服务器

```bash
./cmake-build-debug-wsl/game_server
```

服务端启动后输出：

```text
server listen on port 9000
```

---

### 5. 启动客户端

```bash
./cmake-build-debug-wsl/game_client
```

---

## 六、测试结果

### 50 客户端压测

测试配置：

```text
客户端数量：50
房间人数：2
预期房间数量：25
```

服务端峰值状态：

```text
[ServerStats] online=50 match_queue=0 room=25
```

说明：

```text
50 个客户端全部登录成功
50 个客户端全部加入匹配
每 2 个客户端创建 1 个房间
最终创建 25 个房间
匹配队列无残留
```

客户端全部断开后：

```text
[ServerStats] online=0 match_queue=0 room=0
```

说明：

```text
PlayerManager 清理成功
MatchQueue 清理成功
RoomManager 清理成功
空房间全部删除
```

---

## 七、核心日志示例

### 登录 + 匹配 + 房间创建

```text
[login] login success, player_id: 10000
[match] player 10000 join queue success
[match] match size=1

[login] login success, player_id: 10001
[match] player 10001 join queue success
[match] match size=2

[room] create room=1 player=10000 10001
```

### 房间 Tick

```text
[RoomTick] room_id=1 tick=50 player_count=2
[RoomTick] room_id=1 tick=100 player_count=2
```

### 聊天广播

```text
[ChatNotify] room_id=1 player_id=10000 text=hello from 10000
[ChatNotify] room_id=1 player_id=10001 text=hello from 10001
```

### 断线清理

```text
[disconnect] player_id=10000
[session] closed player_id=10000
[room] remove player_id=10000 room_id=1

[disconnect] player_id=10001
[session] closed player_id=10001
[room] remove player_id=10001 room_id=1
[room] remove empty room:1
```

---

## 八、模块说明

### MessageCodec

负责 TCP 消息编解码。

协议格式：

```text
| body_len: uint32_t | msg_id: uint16_t | body: protobuf bytes |
```

职责：

* encode
* try_decode
* 处理 TCP 粘包 / 拆包
* 限制最大包体大小

---

### Session

负责单个客户端连接。

职责：

* socket 生命周期管理
* async_read_some
* async_write
* recv buffer 累积
* write queue 发送队列
* 断线 close callback
* 保存 player_id

---

### MessageDispatcher

负责根据 `msg_id` 分发消息。

职责：

* 注册 handler
* 分发消息
* 未知 msg_id 日志输出

---

### PlayerManager

负责在线玩家管理。

职责：

* player_id -> Session
* 添加在线玩家
* 移除在线玩家
* 查询在线人数
* 根据 player_id 获取 Session

---

### MatchQueue

负责匹配队列。

职责：

* 玩家加入匹配
* 玩家取消匹配
* 防止重复加入
* 凑够指定人数后弹出匹配玩家

---

### RoomManager

负责房间生命周期。

职责：

* 创建房间
* 删除房间
* player_id -> room_id 映射
* 根据玩家查房间
* 玩家断线时从房间移除
* 房间为空时销毁房间
* tick_all

---

### Room

负责单个房间内部逻辑。

职责：

* 保存房间玩家
* 房间广播
* 聊天处理
* Tick 更新
* 玩家移除
* 判断房间是否为空

---

## 九、当前版本限制

当前 v1 重点是跑通核心链路，因此暂未实现：

* 数据库账号登录
* Redis 状态存储
* 网关服 / 房间服拆分
* 分布式房间
* AOI
* 帧同步
* 状态同步压缩
* 战斗系统
* 断线重连
* 心跳超时踢出
* KCP / UDP
* 反作弊

---

## 十、后续优化方向

### v1.1

* 心跳检测
* 连接超时踢出
* 更完整的客户端模拟器统计
* 日志模块封装
* 配置文件读取端口 / tick interval / room size

### v2

* 网关服和房间服拆分
* Redis 保存在线状态
* 房间状态同步
* 简单战斗逻辑
* 断线重连
* 多线程 io_context
* 房间分线程调度

---

## 十一、项目总结

本项目实现了一个完整的房间制游戏服务器 Demo，覆盖了游戏服务器最核心的链路：

```text
连接 -> 登录 -> 在线管理 -> 匹配 -> 房间 -> 广播 -> Tick -> 断线清理
```

项目重点不是复杂玩法，而是验证游戏服务器基础架构和网络通信流程。当前版本已经通过 50 客户端模拟测试，能够正确创建 25 个房间，并在客户端全部断开后完成资源回收。
