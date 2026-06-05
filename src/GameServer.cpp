//
// Created by xntan on 2026/6/5.
//

#include "GameServer.h"

#include <iostream>
void GameServer::start() {

    acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (ec) {
            std::cout << "accept error: " << ec.message() << "\n";
            return ;
        }
        std::make_shared<>(socket)->run();

    });
}

void GameServer::stop() {

}


