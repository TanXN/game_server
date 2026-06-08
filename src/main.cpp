#include <iostream>

#include "Session.h"
#include "MessageDispatcher.h"
#include "Message.h"
#include <boost/asio.hpp>

#include "GameServer.h"


int main() {

    GameServer game_server;
    game_server.start();

}
