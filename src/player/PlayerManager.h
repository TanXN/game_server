//
// Created by xntan on 2026/6/5.
//

#ifndef GAME_SERVER_PLAYERMANAGER_H
#define GAME_SERVER_PLAYERMANAGER_H
#include "net/Session.h"


class PlayerManager {
public:
    void add_player(int player_id, std::shared_ptr<Session> session);
    void remove_player(int player_id);

    std::shared_ptr<Session> get_session(int player_id);
    bool is_online(int player_id) const;

private:
    std::unordered_map<int, std::weak_ptr<Session>> players_;
};


#endif //GAME_SERVER_PLAYERMANAGER_H
