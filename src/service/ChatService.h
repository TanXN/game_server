//
// Created by xntan on 2026/6/5.
//

#ifndef GAME_SERVER_CHATSERVICE_H
#define GAME_SERVER_CHATSERVICE_H
#include "RankingService.h"
#include "room/RoomManager.h"


class ChatService {
public:
    explicit ChatService(RoomManager& room_manager, RankingService& ranking_service);

    void handle_chat(std::shared_ptr<Session> session, const Message& message);

private:
    void send_chat_failed(std::shared_ptr<Session> session, const std::string& reason);

    RoomManager& room_manager_;
    RankingService& ranking_service_;
};


#endif //GAME_SERVER_CHATSERVICE_H
