//
// Created by xntan on 2026/6/8.
//

#include "ConnectionManager.h"
#include <iostream>

void ConnectionManager::check_timeout(std::chrono::seconds timeout) {
    std::vector<std::shared_ptr<Session>> time_outs;
    for (auto session : sessions_) {
        if (session->check_timeout(timeout)) {
            time_outs.emplace_back(session);
        }
    }

    for (auto session : time_outs) {
        std::cout << "[timeout] close session player=" << session->player_id() << std::endl;
        session->close();
    }
}

std::size_t ConnectionManager::connection_count() const {
    return sessions_.size();
}

void ConnectionManager::remove_session(std::shared_ptr<Session> session) {
    std::lock_guard<std::mutex> lk(mutex_);
    sessions_.erase(session);
}


void ConnectionManager::add_session(std::shared_ptr<Session> session) {
    std::lock_guard<std::mutex> lk(mutex_);
    sessions_.insert(session);
}
