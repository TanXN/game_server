//
// Created by xntan on 2026/6/10.
//

#ifndef GAME_SERVER_PLAYERSTATE_H
#define GAME_SERVER_PLAYERSTATE_H

#include <string>

struct PlayerState {
    int player_id = 0;
    std::string name;

    int score = 0;

    bool online = false;
    int room_id = -1;

    int64_t last_login_time_ms = 0;
    int64_t last_logout_time_ms = 0;

};


#endif //GAME_SERVER_PLAYERSTATE_H
