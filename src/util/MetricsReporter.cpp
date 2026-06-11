//
// Created by xntan on 2026/6/10.
//

#include "MetricsReporter.h"

#include <iostream>


MetricsReporter::MetricsReporter(boost::asio::io_context &io,
                                 ServerMetrics &metrics, PlayerManager &player_manager,
                                 ConnectionManager &connection_manager,
                                 MatchQueue &match_queue, RoomManager &room_manager)
        :timer_(io), metrics_(metrics), player_manager_(player_manager), connection_manager_(connection_manager),
        match_queue_(match_queue), room_manager_(room_manager)
{
}

void MetricsReporter::start() {
    schedule_next();
}

void MetricsReporter::report_once() const{
    std::cout << "[Metrics] online=" << metrics_.online_players()
    << " conn=" << metrics_.connection_count()
    << " room=" << metrics_.room_count()
    << " match_queue=" << metrics_.match_queue_size()
    << " recv_qps=" << metrics_.calc_recv_qps()
    << " send_qps=" << metrics_.calc_send_qps()
    << " total_recv=" << metrics_.total_recv_packets()
    << " total_send=" << metrics_.total_send_packets()
    << " login=" << metrics_.login_req_count()
    << " match=" << metrics_.match_req_count()
    << " chat=" << metrics_.chat_req_count()
    << " heartbeat=" << metrics_.heartbeat_req_count()
    << " ranking=" << metrics_.ranking_req_count()
    << " reconnect=" << metrics_.reconnect_req_count()
    << " avg_heartbeat=" << metrics_.avg_heartbeat_rtt_ms() << "ms"
    << std::endl;
}

void MetricsReporter::schedule_next() {
    timer_.expires_after(std::chrono::seconds(1));
    timer_.async_wait([this](const boost::system::error_code ec) {
        if (ec) {
            std::cout << "[MetricsReporter::schedule_next()] " << ec.message() << std::endl;
            return ;
        }
        report_once();
        schedule_next();
    });
}
