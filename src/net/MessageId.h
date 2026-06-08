//
// Created by xntan on 2026/6/5.
//

#ifndef GAME_SERVER_MESSAGEID_H
#define GAME_SERVER_MESSAGEID_H

#include <cstdint>

class MessageId {
public:
    static constexpr uint16_t LoginReq = 1001;
    static constexpr uint16_t LoginResp = 1002;

    static constexpr uint16_t MatchReq = 2001;
    static constexpr uint16_t MatchResp = 2002;

    static constexpr uint16_t RoomCreatedNotify = 3001;

    static constexpr uint16_t ChatReq = 4001;
    static constexpr uint16_t ChatNotify = 4002;
    static constexpr uint16_t ChatResp = 4003;

    static constexpr uint16_t HeartbeatReq = 9001;
    static constexpr uint16_t HeartbeatResp = 9002;
};
#endif //GAME_SERVER_MESSAGEID_H
