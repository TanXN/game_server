//
// Created by xntan on 2026/6/5.
//

#include "LoginService.h"
#include "net/MessageId.h"
#include "proto/message.pb.h"
#include <iostream>

LoginService::LoginService(PlayerManager &player_manager)
:player_manager_(player_manager) {}


int LoginService::generate_player_id() {
    return next_player_id_++;
}

// v1，把body作为username
std::string LoginService::parse_username(const Message &message) {
    return message.body;
}


void LoginService::send_login_success(std::shared_ptr<Session> session, int player_id) {
    Message msg;
    msg.msg_id = static_cast<uint16_t>(MessageId::LoginResp);

    game_server::LoginResp resp;
    resp.set_code(0);
    resp.set_playerid(player_id);
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
    session->set_player_id(player_id);
    player_manager_.add_player(player_id, session);
    connection_manager_.add_session(session);

    std::cout << "[login] login success, player_id: " << player_id << "\n";
    send_login_success(session, player_id);
}

