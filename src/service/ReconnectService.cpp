//
// Created by xntan on 2026/6/9.
//

#include "ReconnectService.h"

#include "MessageCodec.h"
#include "MessageId.h"
#include "proto/message.pb.h"


ReconnectService::ReconnectService(PlayerManager &player_manager,
                                   RoomManager &room_manager,
                                   PlayerStateManager &player_state_manager,
                                   ConnectionManager &connection_manager,
                                   ServerMetrics &metrics
                                   )
        :player_manager_(player_manager),
        room_manager_(room_manager),
        player_state_manager_(player_state_manager),
        connection_manager_(connection_manager),
        metrics_(metrics)
{

}

void ReconnectService::send_reconnect_success(std::shared_ptr<Session> session, int player_id, int room_id) {
    Message msg;
    game_server::ReconnectResp resp;
    resp.set_code(0);
    resp.set_message("reconnect_success");
    resp.set_player_id(player_id);
    resp.set_room_id(room_id);
    resp.SerializeToString(&msg.body);
    msg.msg_id = MessageId::ReconnectResp;

    session->send(msg);

}

void ReconnectService::send_reconnect_failed(std::shared_ptr<Session> session, std::string &reason) {
    Message msg;
    game_server::ReconnectResp resp;
    resp.set_message(reason);
    resp.set_code(1);
    resp.set_player_id(session->player_id());

    resp.SerializeToString(&msg.body);
    msg.msg_id = MessageId::ReconnectResp;
    session->send(msg);
}

void ReconnectService::handle_reconnect(std::shared_ptr<Session> session, const Message &message) {
    metrics_.incr_reconnect_req();
    game_server::ReconnectReq req;
    req.ParseFromString(message.body);

    int player_id = req.player_id();
    std::string token = req.token();
    auto state = player_state_manager_.get_state(player_id);
    if (state == std::nullopt) {
        std::string reason = "player state not found";
        send_reconnect_failed(session, reason);
        return ;
    }
    if (state->token != token) {
        std::string reason = "token is not match";
        send_reconnect_failed(session, reason);
        return ;
    }
    if (state->conn_state != PlayerConnState::Offline) {
        std::string reason = "connection state is not offline";
        send_reconnect_failed(session, reason);
        return ;
    }

    state->conn_state = PlayerConnState::Online;
    session->set_player_id(player_id);
    session->set_token(token);

    player_manager_.add_player(player_id, session);
    connection_manager_.add_session(session);
    room_manager_.mark_player_reconnected(player_id);

    player_state_manager_.mark_online(player_id);

    send_reconnect_success(session, player_id, state->room_id);
    std::cout << "[reconnect] player_id=" << player_id
              << " room_id=" << state->room_id
              << " success"
              << std::endl;

    return ;
}
