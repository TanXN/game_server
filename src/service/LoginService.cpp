//
// Created by xntan on 2026/6/5.
//

#include "LoginService.h"
#include "net/MessageId.h"
#include "proto/message.pb.h"
#include <iostream>

#include "player/PlayerState.h"

LoginService::LoginService(PlayerManager &player_manager, ConnectionManager& connection_manager,
                           PlayerStateManager& player_state_manager,PlayerStateRepository& player_state_repository)
:player_manager_(player_manager),
connection_manager_(connection_manager),
player_state_manager_(player_state_manager),
player_state_repository_(player_state_repository)
{}


int LoginService::generate_player_id() {
    return next_player_id_++;
}

// v1，把body作为username
std::string LoginService::parse_username(const Message &message) {
    return message.body;
}


void LoginService::send_login_success(std::shared_ptr<Session> session, int player_id, std::string& token) {
    Message msg;
    msg.msg_id = static_cast<uint16_t>(MessageId::LoginResp);

    game_server::LoginResp resp;
    resp.set_code(0);
    resp.set_playerid(player_id);
    resp.set_token(token);
    resp.set_message("login_success");
    resp.SerializeToString(&msg.body);


    session->send(msg);
}

void LoginService::send_login_failed(std::shared_ptr<Session> session, const std::string &reason) {
    Message msg;
    msg.msg_id = static_cast<uint16_t>(MessageId::LoginResp);

    game_server::LoginResp resp;
    resp.set_code(1);
    resp.set_message("login_failed");
    resp.SerializeToString(&msg.body);

    session->send(msg);
}


void LoginService::handle_login(std::shared_ptr<Session> session, const Message &message) {
    game_server::LoginReq req;
    req.ParseFromString(message.body);

    std::string username = req.username();
    if (username.empty()) {
        send_login_failed(session, "username is empty");
        return;
    }
    int player_id = generate_player_id();
    std::string token = generate_token(player_id);
    session->set_player_id(player_id);
    session->set_token(token);
    player_manager_.add_player(player_id, session);
    connection_manager_.add_session(session);

    PlayerState state;
    state.player_id = player_id;
    state.name = username;
    state.score = 0;
    state.online = true;
    state.room_id = -1;
    state.last_login_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    player_state_repository_.save_player_state(state);


    std::cout << "[login] login success, player_id: " << player_id << "\n";
    send_login_success(session, player_id, token);
}


std::string LoginService::generate_token(int player_id) {
    return "token_" + std::to_string(player_id);
}
