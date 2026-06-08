//
// Created by xntan on 2026/6/5.
//

#ifndef GAME_SERVER_ROOM_H
#define GAME_SERVER_ROOM_H

#include "net/Message.h"
#include <vector>
#include "player/PlayerManager.h"

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

private:
    int room_id_;
    std::vector<int> player_ids_;
    PlayerManager& player_manager_;


    int tick_count_;
};


#endif //GAME_SERVER_ROOM_H
