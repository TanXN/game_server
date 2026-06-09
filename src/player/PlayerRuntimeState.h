//
// Created by xntan on 2026/6/9.
//

#ifndef GAME_SERVER_PLAYERRUNTIMESTATE_H
#define GAME_SERVER_PLAYERRUNTIMESTATE_H
#include <string>
#include <chrono>

enum PlayerConnState {
    Online,
    Offline,
};

struct PlayerRuntimeState {
    int player_id = 0;
    int room_id = 0;

    PlayerConnState conn_state = PlayerConnState::Online;

    std::string token;

    std::chrono::steady_clock::time_point disconnected_time;
};

#endif //GAME_SERVER_PLAYERRUNTIMESTATE_H
