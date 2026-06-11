//
// Created by xntan on 2026/6/5.
//

#include "ChatService.h"

#include "MessageId.h"
#include "proto/message.pb.h"

ChatService::ChatService(RoomManager &room_manager, RankingService &ranking_service,
    ServerMetrics &metrics)
    :room_manager_(room_manager),
    ranking_service_(ranking_service),
    metrics_(metrics)
{}

void ChatService::send_chat_failed(std::shared_ptr<Session> session, const std::string &reason) {
    Message msg;
    msg.msg_id = MessageId::ChatResp;

    game_server::ChatResp resp;
    resp.set_code(1);
    resp.set_message(reason);
    resp.SerializeToString(&msg.body);

    session->send(msg);
}

void ChatService::handle_chat(std::shared_ptr<Session> session, const Message &message) {
    metrics_.incr_chat_req();
    game_server::ChatReq req;
    req.ParseFromString(message.body);

    int room_id = req.roomid();
    std::string text = req.text();
    int player_id = session->player_id();

    ranking_service_.add_score(player_id, 1);

    auto room = room_manager_.get_room(room_id);
    room->handle_chat(player_id, text);

}


