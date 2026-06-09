//
// Created by xntan on 2026/6/9.
//

#ifndef GAME_SERVER_RECONNECTSERVICE_H
#define GAME_SERVER_RECONNECTSERVICE_H
#include <memory>
#include <google/protobuf/message.h>

#include "Session.h"
#include "player/PlayerManager.h"
#include "player/PlayerStateManager.h"
#include "room/RoomManager.h"


class ReconnectService {
public:
    ReconnectService(PlayerManager& player_manager,
                    RoomManager& room_manager,
                    PlayerStateManager& player_state_manager);

    void handle_reconnect(std::shared_ptr<Session> session, const Message& message);


private:
    void send_reconnect_success(std::shared_ptr<Session> session, int player_id, int room_id);

    void send_reconnect_failed(std::shared_ptr<Session> session, std::string& reason);


    PlayerManager& player_manager_;
    RoomManager& room_manager_;
    PlayerStateManager& player_state_manager_;
};


#endif //GAME_SERVER_RECONNECTSERVICE_H
