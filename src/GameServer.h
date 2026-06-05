//
// Created by xntan on 2026/6/5.
//

#ifndef GAME_SERVER_GAMESERVER_H
#define GAME_SERVER_GAMESERVER_H


#include <boost/asio.hpp>

#include "player/PlayerManager.h"
#include "match/MatchQueue.h"
#include "net/MessageDispatcher.h"
#include "room/RoomManager.h"

using boost::asio::ip::tcp;

class GameServer {
public:
    void start();
    void stop();

private:
    boost::asio::io_context io_;
    tcp::acceptor acceptor_;

    PlayerManager player_manager_;
    RoomManager room_manager_;
    MatchQueue match_qeueue_;
    MessageDispatcher dispacher_;
};


#endif //GAME_SERVER_GAMESERVER_H
