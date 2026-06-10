//
// Created by xntan on 2026/6/5.
//

#ifndef GAME_SERVER_LOGINSERVICE_H
#define GAME_SERVER_LOGINSERVICE_H
#include "player/PlayerManager.h"

#include <string>
#include <atomic>

#include "ConnectionManager.h"
#include "player/PlayerRuntimeState.h"
#include "player/PlayerStateManager.h"
#include "player/PlayerStateRepository.h"

class session;
class playerManager;


class LoginService {
public:
    explicit LoginService(PlayerManager& player_manager, ConnectionManager& connection_manager,
        PlayerStateManager& player_state_manager,PlayerStateRepository& player_state_repository);

    void handle_login(std::shared_ptr<Session> session, const Message& message);

private:
    std::string generate_token(int player_id);

    int generate_player_id();

    std::string parse_username(const Message& message);

    void send_login_success(std::shared_ptr<Session> session, int player_id, std::string& token);

    void send_login_failed(std::shared_ptr<Session> session,
        const std::string& reason);

    PlayerManager& player_manager_;
    ConnectionManager& connection_manager_;
    PlayerStateManager& player_state_manager_;
    PlayerStateRepository& player_state_repository_;

    std::atomic<int> next_player_id_{10000};

};


#endif //GAME_SERVER_LOGINSERVICE_H
