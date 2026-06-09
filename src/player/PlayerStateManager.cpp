//
// Created by xntan on 2026/6/9.
//

#include "PlayerStateManager.h"

#include <iostream>

void PlayerStateManager::mark_online(int player_id) {
    if (states_.contains(player_id)) {
        states_[player_id].conn_state = PlayerConnState::Online;
    }
}

std::vector<int> PlayerStateManager::get_reconnect_timeout_players(std::chrono::seconds timeout) const {
    std::vector<int> players;
    const auto now = std::chrono::steady_clock::now();
    for (auto it : states_) {
        if (it.second.conn_state == PlayerConnState::Offline) {
            auto duration_sec = std::chrono::duration_cast<std::chrono::seconds>(now - it.second.disconnected_time);
            if (duration_sec > timeout) {
                players.push_back(it.first);
            }
        }
    }
    return players;
}

std::optional<PlayerRuntimeState> PlayerStateManager::get_state(int player_id) const {
    if (states_.contains(player_id)) {
        return states_.at(player_id);
    }
    return std::nullopt;
}

void PlayerStateManager::mark_disconnected(int player_id) {
    if (states_.contains(player_id)) {
        states_[player_id].conn_state = PlayerConnState::Offline;
        states_[player_id].disconnected_time = std::chrono::steady_clock::now();
    }
}

void PlayerStateManager::save_online(int player_id, int room_id, const std::string &token) {
    PlayerRuntimeState state;
    state.player_id = player_id;
    state.conn_state = PlayerConnState::Online;
    state.token = token;
    state.room_id = room_id;
    states_.insert({player_id, state});

}

void PlayerStateManager::remove_player(int player_id) {
    states_.erase(player_id);
}

bool PlayerStateManager::check_token(int player_id, const std::string &token) const {
    if (!states_.contains(player_id)) {
        return false;
    }
    const PlayerRuntimeState state = states_.at(player_id);
    return state.token == token;

}
