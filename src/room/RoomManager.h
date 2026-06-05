//
// Created by xntan on 2026/6/5.
//

#ifndef GAME_SERVER_ROOMMANAGER_H
#define GAME_SERVER_ROOMMANAGER_H

#include "room/Room.h"
class RoomManager {
public:
    int create_room(const std::vector<int>& player_ids);
    void remove_room(int room_id);

    std::shared_ptr<Room> get_room(int room_id);
    std::shared_ptr<Room> get_room_by_player(int player_id);

private:
    int next_room_id_ = 1;

    std::unordered_map<int, std::shared_ptr<Room>> rooms_;
    std::unordered_map<int, int> player_room_map_;
};


#endif //GAME_SERVER_ROOMMANAGER_H
