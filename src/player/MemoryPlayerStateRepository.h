//
// Created by xntan on 2026/6/8.
//

#ifndef GAME_SERVER_MEMORYPLAYERSTATEREPOSITORY_H
#define GAME_SERVER_MEMORYPLAYERSTATEREPOSITORY_H

#include <unordered_map>

#include "PlayerStateRepository.h"

class MemoryPlayerStateRepository : public PlayerStateRepository{
public:
    void save_player_state(const PlayerState &state) override;
    std::optional<PlayerState> load_player_state(int player_id) override;
    void mark_online(int player_id) override;
    void mark_offline(int player_id) override;


private:
    std::unordered_map<int, PlayerState> states_;
};


#endif //GAME_SERVER_MEMORYPLAYERSTATEREPOSITORY_H
