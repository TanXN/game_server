//
// Created by xntan on 2026/6/5.
//

#ifndef GAME_SERVER_ROOM_H
#define GAME_SERVER_ROOM_H

#include "net/Message.h"
#include <vector>
#include "player/PlayerManager.h"
#include "player/PlayerRuntimeState.h"


class PlayerManager;

class Room {
public:
    Room(int room_id, std::vector<int> player_ids, PlayerManager& player_manager);

    void broadcast(const Message& msg);
    void broadcast_except(int except_player_id, const Message& message);
    void handle_chat(int player_id, const std::string& text);
    void tick();

    void remove_player(int player_id);

    int room_id() const;

    const std::vector<int>& player_ids() const;

    bool has_player(int player_id);

    void add_player(int player_id);

    bool empty() const;

    std::size_t player_count() const;

    void mark_player_disconnected(int player_id);

    void mark_player_online(int player_id);

    bool is_player_disconnected(int player_id) const;

private:
    int room_id_;
    std::vector<int> player_ids_;

    std::unordered_map<int, PlayerRuntimeState> player_states_;

    PlayerManager& player_manager_;

    bool enable_room_tick_log_ = false;

    int tick_count_ = 0;
};


#endif //GAME_SERVER_ROOM_H
