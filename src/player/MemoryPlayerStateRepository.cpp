//
// Created by xntan on 2026/6/8.
//

#include "MemoryPlayerStateRepository.h"

#include <sys/stat.h>
#include <optional>
#include "PlayerState.h"

void MemoryPlayerStateRepository::save_player_state(const PlayerState& state) {
    std::lock_guard<std::mutex> lock(mutex_);
    states_[state.player_id] = state;
}

std::optional<PlayerState> MemoryPlayerStateRepository::load_player_state(int player_id) {
    if (!states_.contains(player_id)) {
        return std::nullopt;
    }
    return states_.at(player_id);
}

void MemoryPlayerStateRepository::mark_offline(int player_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    states_[player_id].online = false;
}

void MemoryPlayerStateRepository::mark_online(int player_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    states_[player_id].online = true;
}

std::vector<PlayerState> MemoryPlayerStateRepository::get_all_players() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<PlayerState> result;
    result.reserve(states_.size());
    for (auto &it: states_) {
        result.emplace_back(it.second);
    }
    return result;
}

void MemoryPlayerStateRepository::update_score(int player_id, int score) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (states_.contains(player_id)) {
        states_[player_id].score = score;
    }
}

