//
// Created by xntan on 2026/6/5.
//

#ifndef GAME_SERVER_MATCHSERVICE_H
#define GAME_SERVER_MATCHSERVICE_H
#include <memory>

#include "Message.h"
#include "room/RoomManager.h"

class Session;
class MatchQueue;

class MatchService {
public:
    explicit MatchService(MatchQueue& match_queue, RoomManager& room_manager, ServerMetrics& server_metrics);

    void handle_match(std::shared_ptr<Session> session, const Message& message);

    void handle_cancel_match(std::shared_ptr<Session> session, const Message& message);

private:
    void send_match_success(std::shared_ptr<Session> session);

    void send_match_failure(std::shared_ptr<Session> session, const std::string& reason);

    MatchQueue& match_queue_;

    RoomManager& room_manager_;
    ServerMetrics& metrics_;
};


#endif //GAME_SERVER_MATCHSERVICE_H
