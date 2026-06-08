//
// Created by xntan on 2026/6/5.
//

#include "MatchService.h"

#include "MessageId.h"
#include "Session.h"
#include "match/MatchQueue.h"
#include "proto/message.pb.h"
#include "room/RoomManager.h"

MatchService::MatchService(MatchQueue &match_queue, RoomManager &room_manager)
    : match_queue_(match_queue), room_manager_(room_manager) {
}

void MatchService::handle_match(std::shared_ptr<Session> session, const Message &message) {
    game_server::MatchReq req;
    req.ParseFromString(message.body);
    int player_id = req.playerid();
    if (player_id <= 0) {
        send_match_failure(session, "player_id must be non-negative");
        return;
    }
    if (match_queue_.contains(player_id)) {
        send_match_failure(session, "player_id already in match_queue");
        return ;
    }
    match_queue_.join(player_id);
    send_match_success(session);

    std::cout << "[match] player " <<  player_id << " join queue success" << std::endl;
    std::cout << "[match] match size=" << match_queue_.size() << std::endl;


    auto players = match_queue_.try_pop_matched_players(2);

    if (!players.empty()) {
        int room_id = room_manager_.create_room(players);
        std::cout << "[room] create room="<< room_id
        << " player=" ;
        for (auto player : players) {
            std::cout << player << " ";
        }
        std::cout << std::endl;
    }

    return ;
}

void MatchService::handle_cancel_match(std::shared_ptr<Session> session, const Message &message) {
    game_server::MatchCancelReq req;
    req.ParseFromString(message.body);
    int player_id = req.playerid();
    if (player_id <= 0) {
        send_match_failure(session, "player_id must be non-negative");
        return ;
    }
    match_queue_.leave(player_id);
    return ;

}

void MatchService::send_match_success(std::shared_ptr<Session> session) {
    game_server::MatchResp resp;
    resp.set_code(0);
    resp.set_message("match success");
    Message msg;
    msg.msg_id = MessageId::MatchResp;
    resp.SerializeToString(&msg.body);

    session->send(msg);
    return ;
}



void MatchService::send_match_failure(std::shared_ptr<Session> session, const std::string &reason) {
    game_server::MatchResp resp;

    resp.set_code(1);
    resp.set_message(reason);
    Message msg;
    msg.msg_id = MessageId::MatchResp;
    resp.SerializeToString(&msg.body);

    session->send(msg);

    return;
}

