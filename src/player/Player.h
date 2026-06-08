//
// Created by xntan on 2026/6/5.
//

#ifndef GAME_SERVER_PLAYER_H
#define GAME_SERVER_PLAYER_H
#include <cstdint>

#endif //GAME_SERVER_PLAYER_H

#include <cstdint>

struct PlayerState {
    int player_id = 0;
    int room_id = 0;
    int score = 0;
    bool online = false;
    int64_t last_active_time = 0;
};