//
// Created by xntan on 2026/6/5.
//

#include "ChatService.h"

#include "MessageId.h"
#include "proto/message.pb.h"

ChatService::ChatService(RoomManager &room_manager)
    :room_manager_(room_manager)
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
    game_server::ChatReq req;
    req.ParseFromString(message.body);

    int room_id = req.roomid();
    std::string text = req.text();
    int player_id = session->player_id();

    auto room = room_manager_.get_room(room_id);

    Message chat_msg;
    chat_msg.msg_id = MessageId::ChatNotify;
    game_server::ChatNotify notify;
    notify.set_roomid(room_id);
    notify.set_playerid(player_id);
    notify.set_text(text);
    notify.SerializeToString(&chat_msg.body);
    room->broadcast(chat_msg);

}


