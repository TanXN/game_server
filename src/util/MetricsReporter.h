//
// Created by xntan on 2026/6/10.
//

#ifndef GAME_SERVER_METRICSREPORTER_H
#define GAME_SERVER_METRICSREPORTER_H
#include <boost/asio.hpp>

#include "ConnectionManager.h"
#include "ServerMetrics.h"
#include "match/MatchQueue.h"
#include "player/PlayerManager.h"
#include "room/RoomManager.h"

class MetricsReporter {
public:
    MetricsReporter(
        boost::asio::io_context& io,
        ServerMetrics& metrics,
        PlayerManager& player_manager,
        ConnectionManager& connection_manager,
        MatchQueue& match_queue,
        RoomManager& room_manager
    );

    void start();

private:
    void report_once() const;
    void schedule_next();

    boost::asio::steady_timer timer_;

    ServerMetrics& metrics_;
    PlayerManager& player_manager_;
    ConnectionManager& connection_manager_;
    MatchQueue& match_queue_;
    RoomManager& room_manager_;

};


#endif //GAME_SERVER_METRICSREPORTER_H
