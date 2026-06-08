//
// Created by xntan on 2026/6/5.
//

#ifndef GAME_SERVER_MATCHQUEUE_H
#define GAME_SERVER_MATCHQUEUE_H

#include <unordered_set>
#include <deque>
#include <vector>
#include <mutex>

class MatchQueue {
public:
    // 加入匹配队列
    void join(int player_id);
    // 离开匹配队列
    void leave(int player_id);

    bool contains(int player_id) const;

    std::size_t size() const;

    std::vector<int> try_pop_matched_players(std::size_t room_size);

private:
    std::deque<int> queue_;
    std::unordered_set<int> in_queue_;

    mutable std::mutex mutex_;
};


#endif //GAME_SERVER_MATCHQUEUE_H
