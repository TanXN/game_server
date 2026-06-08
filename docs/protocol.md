# 协议文档

## 一、协议概述

本项目使用 TCP 作为传输层协议，业务消息使用 Protobuf 序列化。

由于 TCP 是字节流协议，不存在天然消息边界，因此项目自定义了消息头，用于解决粘包和拆包问题。

完整消息格式：

```text
| body_len: uint32_t | msg_id: uint16_t | body: protobuf bytes |
```

---

## 二、消息头格式

### Header

| 字段       |   大小 | 类型       | 说明             |
| -------- | ---: | -------- | -------------- |
| body_len | 4 字节 | uint32_t | body 长度        |
| msg_id   | 2 字节 | uint16_t | 消息 ID          |
| body     | N 字节 | bytes    | Protobuf 序列化数据 |

Header 总长度：

```text
6 bytes
```

---

## 三、字节序

`body_len` 和 `msg_id` 使用网络字节序，也就是大端序。

示例：

```text
msg_id = 1001
十六进制 = 0x03E9
网络字节序 = 03 E9
```

读取时需要注意 `char` 符号扩展问题，应该先转成 `uint8_t` 再进行位运算。

---

## 四、最大包体限制

当前最大 body 大小：

```text
MAX_BODY_SIZE = 1024 * 1024
```

也就是 1MB。

如果解析到的 `body_len` 超过该限制，应认为是非法包，并关闭连接。

---

## 五、消息解析流程

服务端或客户端收到 TCP 数据后，不直接认为一次 read 就是一条完整消息。

正确流程：

```text
socket read
  -> append 到 recv_buffer
  -> MessageCodec::try_decode
  -> Success: 解析出一条 Message，并从 buffer 删除该包
  -> NeedMore: 数据不够，等待下一次 read
  -> Error: 非法数据，关闭连接
```

---

## 六、Message 结构

业务层统一使用以下结构表示一条消息：

```cpp
struct Message {
    uint16_t msg_id = 0;
    std::string body;
};
```

其中：

```text
msg_id 用于判断 body 应该解析成哪一种 Protobuf 类型
body 是 Protobuf 序列化后的二进制数据
```

注意：Protobuf body 是二进制数据，不能直接当普通字符串打印。日志中应打印 `body_size`，然后在 Protobuf 解析成功后打印字段内容。

推荐日志：

```text
received packet msg_id=3001 body_size=8
[RoomCreatedNotify] room_id=1 players=10000 10001
```

不推荐日志：

```text
body=乱码二进制内容
```

---

## 七、消息 ID 列表

| msg_id | 名称                | 方向               | Protobuf 类型       | 说明     |
| -----: | ----------------- | ---------------- | ----------------- | ------ |
|   1001 | LoginReq          | Client -> Server | LoginReq          | 登录请求   |
|   1002 | LoginResp         | Server -> Client | LoginResp         | 登录响应   |
|   2001 | MatchReq          | Client -> Server | MatchReq          | 加入匹配   |
|   2002 | MatchResp         | Server -> Client | MatchResp         | 匹配响应   |
|   3001 | RoomCreatedNotify | Server -> Client | RoomCreatedNotify | 房间创建通知 |
|   4001 | ChatReq           | Client -> Server | ChatReq           | 聊天请求   |
|   4002 | ChatNotify        | Server -> Client | ChatNotify        | 聊天广播   |

---

## 八、协议常量建议

可以定义为：

```cpp
namespace MessageId {
    constexpr uint16_t LoginReq = 1001;
    constexpr uint16_t LoginResp = 1002;

    constexpr uint16_t MatchReq = 2001;
    constexpr uint16_t MatchResp = 2002;

    constexpr uint16_t RoomCreatedNotify = 3001;

    constexpr uint16_t ChatReq = 4001;
    constexpr uint16_t ChatNotify = 4002;
}
```

或者使用枚举：

```cpp
enum class MsgId : uint16_t {
    LoginReq = 1001,
    LoginResp = 1002,

    MatchReq = 2001,
    MatchResp = 2002,

    RoomCreatedNotify = 3001,

    ChatReq = 4001,
    ChatNotify = 4002
};
```

---

## 九、Protobuf 消息定义示例

`message.proto` 可以参考以下结构：

```proto
syntax = "proto3";

package game_server;

message LoginReq {
  string username = 1;
}

message LoginResp {
  int32 code = 1;
  int32 player_id = 2;
  string message = 3;
}

message MatchReq {
  int32 player_id = 1;
}

message MatchResp {
  int32 code = 1;
  string message = 2;
}

message RoomCreatedNotify {
  int32 room_id = 1;
  repeated int32 player_ids = 2;
}

message ChatReq {
  int32 room_id = 1;
  string text = 2;
}

message ChatNotify {
  int32 room_id = 1;
  int32 player_id = 2;
  string text = 3;
}
```

---

## 十、LoginReq

### 基本信息

| 项目       | 内容               |
| -------- | ---------------- |
| msg_id   | 1001             |
| 方向       | Client -> Server |
| Protobuf | LoginReq         |

### 字段

| 字段       | 类型     | 说明    |
| -------- | ------ | ----- |
| username | string | 玩家用户名 |

### 处理逻辑

```text
客户端发送 username
服务器生成 player_id
服务器保存 player_id -> Session
服务器返回 LoginResp
```

---

## 十一、LoginResp

### 基本信息

| 项目       | 内容               |
| -------- | ---------------- |
| msg_id   | 1002             |
| 方向       | Server -> Client |
| Protobuf | LoginResp        |

### 字段

| 字段        | 类型     | 说明              |
| --------- | ------ | --------------- |
| code      | int32  | 0 表示成功，非 0 表示失败 |
| player_id | int32  | 服务器生成的玩家 ID     |
| message   | string | 响应信息            |

### 示例日志

```text
[Login] login success, player_id=10000
```

---

## 十二、MatchReq

### 基本信息

| 项目       | 内容               |
| -------- | ---------------- |
| msg_id   | 2001             |
| 方向       | Client -> Server |
| Protobuf | MatchReq         |

### 字段

| 字段        | 类型    | 说明    |
| --------- | ----- | ----- |
| player_id | int32 | 玩家 ID |

说明：

当前服务端主要以 `session.player_id()` 为准，客户端传入的 `player_id` 可作为调试字段。

### 处理逻辑

```text
服务器检查玩家是否已登录
加入 MatchQueue
如果队列人数达到 room_size，则创建房间
返回 MatchResp
```

---

## 十三、MatchResp

### 基本信息

| 项目       | 内容               |
| -------- | ---------------- |
| msg_id   | 2002             |
| 方向       | Server -> Client |
| Protobuf | MatchResp        |

### 字段

| 字段      | 类型     | 说明              |
| ------- | ------ | --------------- |
| code    | int32  | 0 表示成功，非 0 表示失败 |
| message | string | 响应信息            |

### 示例日志

```text
[match] message=match success
```

---

## 十四、RoomCreatedNotify

### 基本信息

| 项目       | 内容                |
| -------- | ----------------- |
| msg_id   | 3001              |
| 方向       | Server -> Client  |
| Protobuf | RoomCreatedNotify |

### 字段

| 字段         | 类型             | 说明          |
| ---------- | -------------- | ----------- |
| room_id    | int32          | 房间 ID       |
| player_ids | repeated int32 | 房间内玩家 ID 列表 |

### 触发时机

当匹配队列凑够指定人数后，服务器创建房间，并向房间内所有玩家广播该通知。

### 示例日志

```text
received packet msg_id=3001 body_size=8
[RoomCreatedNotify] room_id=1 players=10000 10001
```

---

## 十五、ChatReq

### 基本信息

| 项目       | 内容               |
| -------- | ---------------- |
| msg_id   | 4001             |
| 方向       | Client -> Server |
| Protobuf | ChatReq          |

### 字段

| 字段      | 类型     | 说明    |
| ------- | ------ | ----- |
| room_id | int32  | 房间 ID |
| text    | string | 聊天内容  |

### 处理逻辑

```text
服务器收到 ChatReq
根据 session.player_id 获取发送者
根据 player_id 查找所在 Room
构造 ChatNotify
广播给房间内所有玩家
```

---

## 十六、ChatNotify

### 基本信息

| 项目       | 内容               |
| -------- | ---------------- |
| msg_id   | 4002             |
| 方向       | Server -> Client |
| Protobuf | ChatNotify       |

### 字段

| 字段        | 类型     | 说明         |
| --------- | ------ | ---------- |
| room_id   | int32  | 房间 ID      |
| player_id | int32  | 发送聊天的玩家 ID |
| text      | string | 聊天内容       |

### 示例日志

```text
received packet msg_id=4002 body_size=23
[ChatNotify] room_id=1 player_id=10000 text=hello from 10000
```

---

## 十七、客户端解析方式

客户端收到消息后，根据 `msg_id` 选择对应 Protobuf 类型。

示例：

```cpp
void Client::handle_message(const Message& msg) {
    switch (msg.msg_id) {
        case MessageId::LoginResp: {
            game_server::LoginResp resp;
            if (!resp.ParseFromString(msg.body)) {
                std::cout << "parse LoginResp failed" << std::endl;
                return;
            }

            player_id_ = resp.player_id();

            std::cout << "[Login] login success, player_id="
                      << player_id_
                      << std::endl;
            break;
        }

        case MessageId::MatchResp: {
            game_server::MatchResp resp;
            if (!resp.ParseFromString(msg.body)) {
                std::cout << "parse MatchResp failed" << std::endl;
                return;
            }

            std::cout << "[match] message="
                      << resp.message()
                      << std::endl;
            break;
        }

        case MessageId::RoomCreatedNotify: {
            game_server::RoomCreatedNotify notify;
            if (!notify.ParseFromString(msg.body)) {
                std::cout << "parse RoomCreatedNotify failed" << std::endl;
                return;
            }

            room_id_ = notify.room_id();

            std::cout << "[RoomCreatedNotify] room_id="
                      << room_id_
                      << " players=";

            for (int i = 0; i < notify.player_ids_size(); ++i) {
                std::cout << notify.player_ids(i) << " ";
            }

            std::cout << std::endl;
            break;
        }

        case MessageId::ChatNotify: {
            game_server::ChatNotify notify;
            if (!notify.ParseFromString(msg.body)) {
                std::cout << "parse ChatNotify failed" << std::endl;
                return;
            }

            std::cout << "[ChatNotify] room_id="
                      << notify.room_id()
                      << " player_id="
                      << notify.player_id()
                      << " text="
                      << notify.text()
                      << std::endl;
            break;
        }

        default:
            std::cout << "unknown msg_id="
                      << msg.msg_id
                      << std::endl;
            break;
    }
}
```

---

## 十八、客户端发送方式

### 发送 LoginReq

```cpp
game_server::LoginReq req;
req.set_username("player_1");

Message msg;
msg.msg_id = MessageId::LoginReq;
req.SerializeToString(&msg.body);

send(msg);
```

---

### 发送 MatchReq

```cpp
game_server::MatchReq req;
req.set_player_id(player_id_);

Message msg;
msg.msg_id = MessageId::MatchReq;
req.SerializeToString(&msg.body);

send(msg);
```

---

### 发送 ChatReq

```cpp
game_server::ChatReq req;
req.set_room_id(room_id_);
req.set_text("hello from " + std::to_string(player_id_));

Message msg;
msg.msg_id = MessageId::ChatReq;
req.SerializeToString(&msg.body);

send(msg);
```

---

## 十九、错误处理规则

### 1. 未知 msg_id

如果 `MessageDispatcher` 找不到对应 handler：

```text
MessageDispatcher::dispatch() no found msg_id:xxxx
```

应忽略该消息或关闭连接，当前 v1 选择打印日志。

---

### 2. Protobuf 解析失败

如果 `ParseFromString()` 返回 false：

```text
parse LoginResp failed
```

说明：

```text
msg_id 和 protobuf 类型不匹配
body 数据损坏
客户端 / 服务端协议版本不一致
```

---

### 3. body_len 非法

如果 `body_len > MAX_BODY_SIZE`：

```text
body size too large
```

应关闭连接，防止异常包攻击或内存异常增长。

---

## 二十、测试协议流程

### 双客户端创建房间

```text
Client 1 -> LoginReq
Server   -> LoginResp

Client 1 -> MatchReq
Server   -> MatchResp

Client 2 -> LoginReq
Server   -> LoginResp

Client 2 -> MatchReq
Server   -> MatchResp

Server   -> RoomCreatedNotify to Client 1
Server   -> RoomCreatedNotify to Client 2
```

### 聊天广播

```text
Client 1 -> ChatReq
Server   -> ChatNotify to Client 1
Server   -> ChatNotify to Client 2

Client 2 -> ChatReq
Server   -> ChatNotify to Client 1
Server   -> ChatNotify to Client 2
```

---

## 二十一、当前协议版本说明

当前协议为 v1，主要服务于房间服务器 Demo。

已支持：

```text
登录
匹配
房间创建通知
聊天
```

暂未支持：

```text
心跳
重连
离开房间
取消匹配
游戏状态同步
玩家位置同步
战斗指令
错误码枚举细化
```
