//
// Created by xntan on 2026/6/9.
//

#ifndef GAME_SERVER_PLAYERSTATEMANAGER_H
#define GAME_SERVER_PLAYERSTATEMANAGER_H
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "PlayerRuntimeState.h"


class PlayerStateManager {
public:
    void save_online(int player_id, int room_id, const std::string& token);

    void mark_disconnected(int player_id);

    void remove_player(int player_id);

    std::optional<PlayerRuntimeState> get_state(int player_id) const;

    bool check_token(int player_id, const std::string& token) const;

    void mark_online(int player_id);

    std::vector<int> get_reconnect_timeout_players(std::chrono::seconds timeout) const;

private:

    mutable std::mutex mutex_;
    std::unordered_map<int, PlayerRuntimeState> states_;

};


#endif //GAME_SERVER_PLAYERSTATEMANAGER_H
