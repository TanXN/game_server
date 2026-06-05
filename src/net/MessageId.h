//
// Created by xntan on 2026/6/5.
//

#ifndef GAME_SERVER_MESSAGEID_H
#define GAME_SERVER_MESSAGEID_H

enum class MessageId {
    LoginReq = 1001,
    LoginResp = 1002,

    MatchReq = 2001,
    MatchResp = 2002,

    RoomCreatedNotify = 3001,
    ChatReq = 4001,
    ChatNotify = 4002,

    HeartbeatReq = 9001,
    HeartbeatResp = 9002,
};
#endif //GAME_SERVER_MESSAGEID_H
