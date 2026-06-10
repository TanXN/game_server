//
// Created by xntan on 2026/6/8.
//

#ifndef GAME_SERVER_MEMORYPLAYERSTATEREPOSITORY_H
#define GAME_SERVER_MEMORYPLAYERSTATEREPOSITORY_H

#include <mutex>
#include <unordered_map>

#include "PlayerStateRepository.h"

class MemoryPlayerStateRepository : public PlayerStateRepository{
public:
    void save_player_state(const PlayerState &state) override;
    std::optional<PlayerState> load_player_state(int player_id) override;
    void mark_online(int player_id) override;
    void mark_offline(int player_id) override;

    void update_score(int player_id, int score) override;

    std::vector<PlayerState> get_all_players() override;

private:
    std::mutex mutex_;
    std::unordered_map<int, PlayerState> states_;
};


#endif //GAME_SERVER_MEMORYPLAYERSTATEREPOSITORY_H
