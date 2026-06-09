//
// Created by xntan on 2026/6/5.
//

#ifndef GAME_SERVER_ROOMMANAGER_H
#define GAME_SERVER_ROOMMANAGER_H

#include "room/Room.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <atomic>


class Room;
class PlayerManager;

class RoomManager {
public:
    explicit RoomManager(PlayerManager& player_manager);

    int create_room(const std::vector<int>& player_ids);
    void remove_room(int room_id);

    std::shared_ptr<Room> get_room(int room_id);
    std::shared_ptr<Room> get_room_by_player(int player_id);

    bool is_player_in_room(int player_id) const;

    int get_room_id_by_player(int player_id) const ;

    void remove_player(int player_id);

    std::size_t room_count() const;

    void tick_all();

    void mark_player_disconnected(int player_id);

    void mark_player_reconnected(int player_id);
private:
    int generate_room_id();

    mutable std::mutex mutex_;

    std::atomic<int> next_room_id_ = 1;

    PlayerManager& player_manager_;

    std::unordered_map<int, std::shared_ptr<Room>> rooms_;
    std::unordered_map<int, int> player_room_map_;
};


#endif //GAME_SERVER_ROOMMANAGER_H
