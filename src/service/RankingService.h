//
// Created by xntan on 2026/6/10.
//

#ifndef GAME_SERVER_RANKINGSERVICE_H
#define GAME_SERVER_RANKINGSERVICE_H

#include <memory>

#include "Message.h"
#include "Session.h"
#include "player/PlayerStateRepository.h"

class RankingService {
public:
    explicit RankingService(PlayerStateRepository& player_state_repo,
                            ServerMetrics& server_metrics
    );

    void handle_ranking_req(std::shared_ptr<Session> session, const Message& message);

    void add_score(int player_id, int delta_score);

private:
    void send_ranking_resp(std::shared_ptr<Session> session);

    PlayerStateRepository& player_state_repo_;
    ServerMetrics& metrics_;

};


#endif //GAME_SERVER_RANKINGSERVICE_H
