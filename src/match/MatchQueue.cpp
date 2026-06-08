//
// Created by xntan on 2026/6/5.
//

#include "MatchQueue.h"


#include <algorithm>


bool MatchQueue::contains(int player_id) const {
    return in_queue_.contains(player_id);
}

void MatchQueue::join(int player_id) {
    queue_.push_back(player_id);
    in_queue_.insert(player_id);
}

void MatchQueue::leave(int player_id) {
    if (!in_queue_.contains(player_id)) {
        return ;
    }
    in_queue_.erase(player_id);

    for (auto it = queue_.begin(); it != queue_.end(); ++it) {
        if (*it == player_id) {
            queue_.erase(it);
            return;
        }
    }
}

std::size_t MatchQueue::size() const {
    return queue_.size();
}

std::vector<int> MatchQueue::try_pop_matched_players(std::size_t room_size) {
    if (queue_.size() < room_size) {
        return std::vector<int>();
    }
    std::vector<int> matched_players;
    for (int i = 0; i < room_size; i++) {
        int player_id = queue_.front();
        matched_players.push_back(player_id);
        queue_.pop_front();
        in_queue_.erase(player_id);
    }

    return matched_players;
}
