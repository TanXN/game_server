//
// Created by xntan on 2026/6/5.
//

#ifndef GAME_SERVER_GAMESERVER_H
#define GAME_SERVER_GAMESERVER_H


#include <boost/asio.hpp>

#include "ConnectionManager.h"
#include "player/PlayerManager.h"
#include "match/MatchQueue.h"
#include "net/MessageDispatcher.h"
#include "player/MemoryPlayerStateRepository.h"
#include "player/PlayerStateManager.h"
#include "room/RoomManager.h"
#include "service/ChatService.h"
#include "service/HeartbeatService.h"

#include "service/LoginService.h"
#include "service/MatchService.h"
#include "service/ReconnectService.h"
#include "util/MetricsReporter.h"
using boost::asio::ip::tcp;

class GameServer {
public:
    void start();
    void stop();
    GameServer();

private:
    void start_tick();
    void do_accept(tcp::acceptor& acceptor, MessageDispatcher& dispatcher);
    void print_stat();
    void start_timeout_check();
    void start_reconnect_timeout_check();
    void check_reconnect_timeout();


    boost::asio::io_context io_;
    tcp::acceptor acceptor_;

    PlayerManager player_manager_;
    RoomManager room_manager_;
    MatchQueue match_queue_;
    MessageDispatcher dispatcher_;
    ConnectionManager connection_manager_;
    PlayerStateManager player_state_manager_;
    MemoryPlayerStateRepository player_state_repository_;

    ServerMetrics metrics_;
    MetricsReporter metrics_reporter_;

    LoginService login_service_;
    MatchService match_service_;
    RankingService ranking_service_;
    ChatService chat_service_;
    HeartbeatService heartbeat_service_;
    ReconnectService reconnect_service_;

    boost::asio::steady_timer timer_;
    boost::asio::steady_timer print_timer_;
    boost::asio::steady_timer heartbeat_timer_;
    boost::asio::steady_timer check_reconnect_timer_;



};


#endif //GAME_SERVER_GAMESERVER_H
