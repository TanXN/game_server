//
// Created by xntan on 2026/6/5.
//

#include "PlayerManager.h"


std::shared_ptr<Session> PlayerManager::get_session(int player_id) {
    return players_.at(player_id).lock();

}

bool PlayerManager::is_online(int player_id) const {
    return players_.contains(player_id);

}

std::size_t PlayerManager::online_count() const {
    return players_.size();
}


void PlayerManager::remove_player(int player_id) {
    players_.erase(player_id);
}


void PlayerManager::add_player(int player_id, std::shared_ptr<Session> session) {
    players_.insert_or_assign(player_id, session);
}

