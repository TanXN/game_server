//
// Created by xntan on 2026/6/5.
//

#include "RoomManager.h"
#include <mutex>

#include "MessageId.h"
#include "proto/message.pb.h"

void RoomManager::remove_player(int player_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!player_room_map_.contains(player_id)) {
        return ;
    }
    int roomId = get_room_id_by_player(player_id);
    auto room = get_room(roomId);
    room->remove_player(player_id);
    player_room_map_.erase(player_id);

    return ;
}


int RoomManager::generate_room_id() {
    return next_room_id_++;
}

std::shared_ptr<Room> RoomManager::get_room(int room_id) {
    if (!rooms_.contains(room_id)) {
        return nullptr;
    }
    return rooms_.at(room_id);
}


std::shared_ptr<Room> RoomManager::get_room_by_player(int player_id) {
    if (!player_room_map_.contains(player_id)) {
        return nullptr;
    }
    int room_id = get_room_id_by_player(player_id);
    return get_room(room_id);
}

int RoomManager::get_room_id_by_player(int player_id) const {
    if (!player_room_map_.contains(player_id)) {
        return -1;
    }
    return player_room_map_.at(player_id);
}


bool RoomManager::is_player_in_room(int player_id) const {
    return player_room_map_.contains(player_id);
}

void RoomManager::remove_room(int room_id) {
    rooms_.erase(room_id);

    for (auto it=player_room_map_.begin();it!=player_room_map_.end();) {
        if (it->second == room_id) {
            it = player_room_map_.erase(it);
        }else {
            ++it;
        }
    }
}

RoomManager::RoomManager(PlayerManager &player_manager)
    :player_manager_(player_manager)
{}

void RoomManager::tick_all() {
    for (auto it : rooms_) {
        it.second->tick();
    }
}

std::size_t RoomManager::room_count() const {
    return rooms_.size();
}

int RoomManager::create_room(const std::vector<int> &player_ids) {
    std::lock_guard<std::mutex> lock(mutex_);
    int room_id = generate_room_id();
    Room room(room_id, player_ids, player_manager_);

    rooms_.insert({room_id, std::make_shared<Room>(room)});
    for (auto player_id : player_ids) {
        player_room_map_.insert({player_id, room_id});
    }

    Message msg;
    msg.msg_id = MessageId::RoomCreatedNotify;

    game_server::RoomCreatedNotify resp;
    resp.set_roomid(room_id);
    for (int i=0;i<player_ids.size();i++) {
        resp.add_players(player_ids.at(i));
    }
    resp.SerializeToString(&msg.body);
    for (auto player_id : player_ids) {
        auto session = player_manager_.get_session(player_id);
        session->send(msg);
    }

    return room_id;
}
