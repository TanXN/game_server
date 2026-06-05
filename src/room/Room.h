//
// Created by xntan on 2026/6/5.
//

#ifndef GAME_SERVER_ROOM_H
#define GAME_SERVER_ROOM_H

#include "net/Message.h"

class Room {
public:
    Room(int room_id, std::vector<int> player_ids);

    void broadcast(const Message& msg);
    void handle_chat(int player_id, const std::string& text);
    void tick();
    void remove_player(int player_id);

private:
    int room_id_;
    std::vector<int> player_ids_;

};


#endif //GAME_SERVER_ROOM_H
