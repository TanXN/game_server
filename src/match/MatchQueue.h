//
// Created by xntan on 2026/6/5.
//

#ifndef GAME_SERVER_MATCHQUEUE_H
#define GAME_SERVER_MATCHQUEUE_H

#include <unordered_map>

class MatchQueue {
public:
    void join(int player_id);
    void leave(int player_id);
    void update();

private:
    std::deque<int> queue_;
    std::unordered_map<int> in_queue_;

};


#endif //GAME_SERVER_MATCHQUEUE_H
