//
// Created by xntan on 2026/6/5.
//

#include "Room.h"

#include <iostream>

#include "MessageId.h"
#include "player/PlayerManager.h"
#include "proto/message.pb.h"

Room::Room(int room_id, std::vector<int> player_ids, PlayerManager& player_manager)
    :room_id_(room_id), player_ids_(player_ids), player_manager_(player_manager)
{}

void Room::add_player(int player_id) {
    player_ids_.emplace_back(player_id);
}

void Room::broadcast(const Message &msg) {
    for (int i=0; i<player_ids_.size(); i++) {
        int player_id = player_ids_[i];
        auto session = player_manager_.get_session(player_id);
        session->send(msg);
    }
}

bool Room::empty() const {
    return player_ids_.empty();
}

bool Room::has_player(int player_id) {
    for (int i = 0; i < player_ids_.size(); i++) {
        if (player_ids_[i] == player_id) {
            return true;
        }
    }
    return false;
}

std::size_t Room::player_count() const {
    return player_ids_.size();
}

void Room::tick() {
    ++tick_count_;

    if (tick_count_ % 50 == 0) {
        std::cout << "[RoomTick] room_id=" << room_id_
        << " tick=" << tick_count_
        << " player_count=" << player_ids_.size()
        << std::endl;
    }
}

void Room::handle_chat(int player_id, const std::string &text) {
    Message chat_msg;
    chat_msg.msg_id = MessageId::ChatNotify;

    game_server::ChatNotify notify;
    notify.set_roomid(room_id_);
    notify.set_playerid(player_id);
    notify.set_text(text);
    notify.SerializeToString(&chat_msg.body);
    broadcast(chat_msg);
}

const std::vector<int> &Room::player_ids() const {
    return player_ids_;
}

void Room::remove_player(int player_id) {
    for (auto it = player_ids_.begin(); it != player_ids_.end(); it++) {
        if (*it == player_id) {
            player_ids_.erase(it);
            return ;
        }
    }
}

int Room::room_id() const {
    return room_id_;
}


void Room::broadcast_except(int except_player_id, const Message &message) {
    for (int i=0; i<player_ids_.size(); i++) {
        int player_id = player_ids_[i];
        if (player_id != except_player_id) {
            auto session = player_manager_.get_session(player_id);
            session->send(message);
        }
    }
}
