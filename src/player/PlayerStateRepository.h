//
// Created by xntan on 2026/6/8.
//

#ifndef GAME_SERVER_PLAYERSTATEREPOSITORY_H
#define GAME_SERVER_PLAYERSTATEREPOSITORY_H
#include <optional>
#include "PlayerState.h"
#include <vector>

class PlayerStateRepository {
public:
    virtual ~PlayerStateRepository() = default;

    virtual void save_player_state(const PlayerState& state) = 0;
    virtual std::optional<PlayerState> load_player_state(int player_id) = 0;
    virtual void mark_online(int player_id) = 0;
    virtual void mark_offline(int player_id) = 0;

    virtual void update_score(int player_id, int score) = 0;

    virtual std::vector<PlayerState> get_all_players() = 0;
};


#endif //GAME_SERVER_PLAYERSTATEREPOSITORY_H
