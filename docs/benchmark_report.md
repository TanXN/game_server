# 压测报告：C++ 房间服务器 v2

## 一、测试目标

本次压测用于验证 C++ 房间服务器 v2 在多客户端并发场景下的稳定性和基础性能。

重点验证以下能力：

* 多客户端并发连接
* 登录流程
* 匹配队列
* 房间创建
* 房间广播
* 聊天同步
* 心跳保活
* 排行榜请求
* 服务端 QPS 统计
* 在线人数、连接数、房间数统计
* 平均心跳 RTT 统计
* 匹配队列是否有残留

---

## 二、测试环境

| 项目    | 配置            |
| ----- | ------------- |
| 操作系统  | WSL Ubuntu    |
| 开发环境  | CLion + CMake |
| 编译模式  | Debug         |
| 编程语言  | C++17         |
| 网络库   | Boost.Asio    |
| 序列化   | Protobuf      |
| 传输协议  | TCP           |
| 服务端端口 | 9000          |
| 测试方式  | 本机多客户端模拟      |

说明：

当前测试在本机 / WSL 环境下进行，网络延迟主要来自进程调度、事件循环处理和本地回环通信，不代表真实公网环境延迟。

---

## 三、服务端核心模块

本次压测覆盖以下服务端模块：

```text
MessageCodec
Session
MessageDispatcher
PlayerManager
ConnectionManager
MatchQueue
RoomManager
Room
LoginService
MatchService
ChatService
HeartbeatService
RankingService
ReconnectService
PlayerStateRepository
ServerMetrics
MetricsReporter
```

---

## 四、协议与业务流程

### 1. 客户端启动流程

每个客户端启动后执行以下流程：

```text
connect server
  -> LoginReq
  -> LoginResp
  -> MatchReq
  -> MatchResp
  -> RoomCreatedNotify
  -> ChatReq
  -> ChatNotify
  -> HeartbeatReq / HeartbeatResp
  -> RankingReq / RankingResp
```

---

### 2. 房间规则

本次测试采用 2 人房间：

```text
room_size = 2
```

因此：

```text
50 个客户端 -> 25 个房间
```

---

### 3. 聊天广播规则

客户端发送一条 `ChatReq` 后，服务端会向房间内所有玩家广播 `ChatNotify`。

因为每个房间有 2 个玩家，所以：

```text
1 个 ChatReq -> 2 个 ChatNotify
```

这也是压测中 `send_qps` 经常大于 `recv_qps` 的主要原因。

---

### 4. 排行榜规则

当前 v2 使用内存版玩家状态仓库和排行榜逻辑。

测试规则：

```text
每次 ChatReq 成功处理后，发送者 score +1
RankingReq 查询当前分数 TopN
```

---

## 五、测试配置

| 项目               |    数值 |
| ---------------- | ----: |
| 模拟客户端数量          |    50 |
| 房间人数             |     2 |
| 预期房间数量           |    25 |
| 登录请求数量           |    50 |
| 匹配请求数量           |    50 |
| 排行榜请求数量          |    50 |
| 心跳开启             |     是 |
| 心跳 RTT 统计        |     是 |
| 重连测试             | 本轮未开启 |
| 服务端 Metrics 输出周期 | 约 1 秒 |

---

## 六、统计指标说明

服务端通过 `ServerMetrics` 和 `MetricsReporter` 输出以下指标：

| 指标                | 说明            |
| ----------------- | ------------- |
| online            | 当前在线玩家数量      |
| conn              | 当前连接数量        |
| room              | 当前房间数量        |
| match_queue       | 当前匹配队列人数      |
| recv_qps          | 当前周期内服务端接收包数量 |
| send_qps          | 当前周期内服务端发送包数量 |
| total_recv        | 服务端累计接收包数量    |
| total_send        | 服务端累计发送包数量    |
| login             | 累计登录请求数量      |
| match             | 累计匹配请求数量      |
| chat              | 累计聊天请求数量      |
| heartbeat         | 累计心跳请求数量      |
| ranking           | 累计排行榜请求数量     |
| reconnect         | 累计重连请求数量      |
| avg_heartbeat_rtt | 平均心跳 RTT      |

---

## 七、测试结果摘要

### 1. 房间创建结果

压测过程中，50 个客户端全部完成登录和匹配。

服务端成功创建 25 个房间：

```text
online=50
conn=50
room=25
match_queue=0
```

说明：

```text
50 个客户端全部连接成功
50 个客户端全部登录成功
50 个客户端全部进入匹配
每 2 个客户端创建 1 个房间
最终创建 25 个房间
匹配队列没有残留
```

---

### 2. 初始请求统计

服务端关键日志：

```text
[Metrics] online=50 conn=50 room=25 match_queue=0 recv_qps=150 send_qps=250 total_recv=150 total_send=250 login=50 match=50 chat=50 heartbeat=0 ranking=0 reconnect=0 avg_heartbeat_rtt=0 ms
```

该结果与预期一致。

#### 接收包数量分析

每个客户端初始发送：

```text
1 个 LoginReq
1 个 MatchReq
1 个 ChatReq
```

因此：

```text
50 * 3 = 150
```

对应日志：

```text
total_recv=150
login=50
match=50
chat=50
```

#### 发送包数量分析

服务端初始发送：

```text
50 个 LoginResp
50 个 MatchResp
50 个 RoomCreatedNotify
100 个 ChatNotify
```

其中：

```text
RoomCreatedNotify = 25 个房间 * 每房间 2 人 = 50
ChatNotify = 50 个 ChatReq * 每房间广播 2 人 = 100
```

因此：

```text
50 + 50 + 50 + 100 = 250
```

对应日志：

```text
total_send=250
```

---

### 3. 稳定运行阶段

压测稳定运行阶段，服务端持续处理聊天、心跳和排行榜请求。

典型日志如下：

```text
[Metrics] online=50 conn=50 room=25 match_queue=0 recv_qps=75 send_qps=100 total_recv=1100 total_send=1700 login=50 match=50 chat=550 heartbeat=400 ranking=50 reconnect=0 avg_heartbeat_rtt=2.46452 ms
```

该结果说明：

```text
50 个客户端保持在线
50 个连接保持活跃
25 个房间持续存在
匹配队列无残留
聊天请求持续处理
心跳请求持续处理
排行榜请求已完成
平均心跳 RTT 约 2~3ms
```

---

## 八、关键指标结果

| 指标          |                   结果 |
| ----------- | -------------------: |
| 峰值在线人数      |                   50 |
| 峰值连接数       |                   50 |
| 房间数量        |                   25 |
| 匹配队列残留      |                    0 |
| 登录请求数量      |                   50 |
| 匹配请求数量      |                   50 |
| 聊天请求数量      |                 550+ |
| 心跳请求数量      |                 400+ |
| 排行榜请求数量     |                   50 |
| 重连请求数量      |                    0 |
| 平均心跳 RTT    |              约 2~3ms |
| 典型 recv_qps |   25 / 50 / 75 / 150 |
| 典型 send_qps | 50 / 100 / 150 / 250 |

---

## 九、QPS 结果分析

### 1. 初始阶段

初始阶段出现：

```text
recv_qps=150
send_qps=250
```

这是因为 50 个客户端在短时间内集中完成登录、匹配和初始聊天。

### 2. 聊天阶段

稳定运行过程中经常出现：

```text
recv_qps=25
send_qps=50
```

原因是：

```text
25 个 ChatReq -> 50 个 ChatNotify
```

每个聊天请求会广播给房间内 2 个玩家，因此服务端发包数量约为收包数量的 2 倍。

### 3. 心跳阶段

心跳集中到达时，可能出现：

```text
recv_qps=50
send_qps=50
```

原因是：

```text
50 个 HeartbeatReq -> 50 个 HeartbeatResp
```

心跳是一对一响应，不涉及房间广播。

---

## 十、排行榜测试结果

本轮压测中，50 个客户端均发起排行榜请求。

服务端日志：

```text
ranking=50
```

说明：

```text
50 个 RankingReq 全部处理完成
RankingService 可以正常从 PlayerStateRepository 获取玩家分数
RankingResp 可以正常返回客户端
```

当前排行榜为内存版实现，后续可以替换为 Redis ZSET。

---

## 十一、心跳 RTT 结果

压测过程中，平均心跳 RTT 稳定在毫秒级：

```text
avg_heartbeat_rtt=1 ms
avg_heartbeat_rtt=3.34021 ms
avg_heartbeat_rtt=3.10185 ms
avg_heartbeat_rtt=2.816 ms
avg_heartbeat_rtt=2.46452 ms
```

说明：

```text
50 客户端本机并发情况下，心跳请求和响应延迟较低
服务端事件循环可以正常处理心跳包
```

注意：

当前测试为本机环境，RTT 结果不能代表公网环境。真实部署时需要在不同机器或云服务器环境下重新测试。

---

## 十二、测试结论

本轮 50 客户端压测通过。

服务端在当前测试规模下表现正常：

```text
50 个客户端全部连接成功
50 个客户端全部登录成功
50 个客户端全部匹配成功
25 个房间创建成功
匹配队列无残留
聊天广播正常
心跳正常
排行榜请求正常
QPS 统计正常
平均心跳 RTT 正常
```

当前 v2 已验证以下能力：

```text
异步 TCP 通信
自定义协议编解码
Protobuf 消息解析
登录模拟
在线玩家管理
匹配队列
房间创建
房间广播
聊天同步
心跳保活
排行榜
玩家状态保存
服务端 Metrics 统计
50 客户端并发测试
```

---

## 十三、发现的问题

### 1. 压测日志过多

压测过程中仍然存在较多业务日志，例如：

```text
[login]
[match]
[room]
[ranking]
[heartbeat]
[RoomTick]
```

这些日志会影响压测性能，也会干扰观察核心指标。

建议压测模式下只保留：

```text
[Metrics]
```

---

### 2. RoomTick 日志过于频繁

当前房间 Tick 会周期性打印每个房间状态。
在 25 个房间下，日志量已经比较明显。

建议：

```text
压测模式关闭 RoomTick 日志
调试模式保留 RoomTick 日志
```

---

### 3. 心跳日志过多

50 客户端心跳会产生大量：

```text
[heartbeat] recv from player ...
```

建议压测模式下关闭单条心跳日志，只保留累计指标：

```text
heartbeat=xxx
avg_heartbeat_rtt=xxx ms
```

---

### 4. 当前为 Debug 构建

本轮测试使用 Debug 构建，性能结果不能代表 Release 性能。

建议后续补充 Release 构建压测：

```bash
cmake -S . -B cmake-build-release -DCMAKE_BUILD_TYPE=Release
cmake --build cmake-build-release
```

---

## 十四、优化方向

### 1. 日志优化

增加日志等级：

```text
DEBUG
INFO
WARN
ERROR
```

压测时只输出：

```text
INFO Metrics
WARN
ERROR
```

---

### 2. Release 压测

后续应在 Release 模式下重新测试：

```text
50 客户端
100 客户端
200 客户端
500 客户端
```

并记录：

```text
峰值连接数
平均 QPS
最大 QPS
平均 RTT
资源回收情况
```

---

### 3. 客户端压测统计

当前主要统计服务端指标。
后续可以在客户端增加：

```text
connect_success
connect_failed
login_success
match_success
room_created_count
chat_notify_count
heartbeat_success
ranking_success
avg_rtt
```

---

### 4. Redis 排行榜

当前排行榜为内存版实现。

后续可以使用 Redis ZSET 替换：

```text
ZADD ranking:score score player_id
ZREVRANGE ranking:score 0 9 WITHSCORES
```

这样排行榜可以支持跨进程共享和持久化。

---

### 5. MySQL 玩家状态持久化

当前玩家状态为内存保存。

后续可接入 MySQL 保存长期数据：

```text
player_id
username
score
last_login_time
last_logout_time
```

实时在线状态仍建议放在内存或 Redis 中。

---

### 6. 多线程 io_context

当前版本主要使用单 io_context。

后续可以扩展为：

```text
多个 worker 线程运行 io_context
房间逻辑与网络 IO 分离
业务任务投递到逻辑线程执行
```

---

## 十五、后续测试计划

### 1. 100 客户端测试

目标：

```text
online=100
conn=100
room=50
match_queue=0
```

### 2. 200 客户端测试

目标：

```text
online=200
conn=200
room=100
match_queue=0
```

### 3. 断线重连压测

测试内容：

```text
部分客户端停止心跳
服务端标记 Disconnected
客户端在重连窗口内恢复
验证 room_id 恢复成功
```

### 4. 重连超时清理测试

测试内容：

```text
客户端断线后不重连
超过重连窗口
服务端清理 PlayerState 和 Room
最终 online=0 room=0
```

---

## 十六、报告结论

本次 50 客户端压测验证了 C++ 房间服务器 v2 的核心稳定性。

在当前测试规模下，服务端可以稳定维护：

```text
50 个在线玩家
50 个 TCP 连接
25 个游戏房间
0 个匹配队列残留
```

服务端可以正常处理：

```text
登录
匹配
房间创建
聊天广播
心跳
排行榜
Metrics 统计
```

平均心跳 RTT 约为 2~3ms，服务端 QPS 统计与业务请求数量匹配，说明当前网络层、协议层和业务层链路基本稳定。

该版本可以作为 C++ 房间服务器 v2 的阶段性压测结果。
