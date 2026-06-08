//
// Created by xntan on 2026/6/8.
//

#include "ConnectionManager.h"

void ConnectionManager::check_timeout(std::chrono::seconds timeout) {
    for (auto session : sessions_) {
        session->check_timeout(timeout);
    }
}

std::size_t ConnectionManager::connection_count() const {

}

void ConnectionManager::remove_session(std::shared_ptr<Session> session) {

}


void ConnectionManager::add_session(std::shared_ptr<Session> session) {

}
