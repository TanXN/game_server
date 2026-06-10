//
// Created by xntan on 2026/6/10.
//

#include "RankingService.h"

#include "MessageId.h"
#include "player/PlayerState.h"
#include "proto/message.pb.h"

void RankingService::add_score(int player_id, int delta_score) {
    auto state = player_state_repo_.load_player_state(player_id);
    if (state.has_value()) {
        player_state_repo_.update_score(player_id, state.value().score + delta_score);
    }
}

void RankingService::send_ranking_resp(std::shared_ptr<Session> session) {

}

RankingService::RankingService(PlayerStateRepository &player_state_repo)
    :player_state_repo_(player_state_repo)
{

}

void RankingService::handle_ranking_req(std::shared_ptr<Session> session, const Message &message) {
    game_server::RankingReq req;
    req.ParseFromString(message.body);
    int top_n = req.top_n();
    auto players = player_state_repo_.get_all_players();
    std::cout << "[ranking] top_n=" << top_n << std::endl;
    std::sort(players.begin(), players.end(),[](const PlayerState& a, const PlayerState& b) {
        return a.score > b.score;
    });

    game_server::RankingResp resp;
    int size = std::min(top_n, static_cast<int>(players.size()));
    for (int i=0; i<size; i++) {
        auto item = resp.add_items();
        item->set_player_id(players[i].player_id);
        item->set_score(players[i].score);
        std::cout << "[ranking] player_id=" << players[i].player_id
        << " score=" << players[i].score << std::endl;
    }

    Message msg;
    msg.msg_id = MessageId::RankingResp;
    resp.SerializeToString(&msg.body);

    session->send(msg);
}
