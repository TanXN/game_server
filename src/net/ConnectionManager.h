//
// Created by xntan on 2026/6/8.
//

#ifndef GAME_SERVER_CONNECTIONMANAGER_H
#define GAME_SERVER_CONNECTIONMANAGER_H
#include <memory>
#include <unordered_set>

#include "Session.h"

class ConnectionManager {
public:
    void add_session(std::shared_ptr<Session> session);

    void remove_session(std::shared_ptr<Session> session);

    void check_timeout(std::chrono::seconds timeout);

    std::size_t connection_count()const;
private:
    std::mutex mutex_;
    std::unordered_set<std::shared_ptr<Session>> sessions_;
};


#endif //GAME_SERVER_CONNECTIONMANAGER_H
