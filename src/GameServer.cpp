//
// Created by xntan on 2026/6/5.
//

#include "GameServer.h"

#include <iostream>
#include "net/MessageId.h"
#include "service/LoginService.h"
#include "room/RoomManager.h"

GameServer::GameServer()
    :acceptor_(io_, tcp::endpoint(tcp::v4(), 9000)),
    room_manager_(player_manager_),
    login_service_(player_manager_),
    match_service_(match_queue_, room_manager_),
    chat_service_(room_manager_),
    timer_(io_),
    print_timer_(io_)
{

}

void GameServer::start() {

    dispatcher_.register_handler(static_cast<uint16_t>(MessageId::LoginReq),
        [this](const std::shared_ptr<Session> &session, const Message& message) {
            login_service_.handle_login(session, message);
        });
    dispatcher_.register_handler(static_cast<uint16_t>(MessageId::MatchReq),
        [this](const std::shared_ptr<Session> &session, const Message& message) {
            match_service_.handle_match(session, message);
        });
    dispatcher_.register_handler(static_cast<uint16_t>(MessageId::ChatReq),
        [this](const std::shared_ptr<Session> &session, const Message& message) {
            chat_service_.handle_chat(session, message);
        });


    std::cout << "server listen on port 9000" << std::endl;
    do_accept(acceptor_, dispatcher_);
    start_tick();
    print_stat();
    io_.run();

}

void GameServer::stop() {

}

void GameServer::do_accept(tcp::acceptor &acceptor, MessageDispatcher &dispatcher) {
    acceptor.async_accept([&](boost::system::error_code ec, tcp::socket socket) {
        if (ec) {
            std::cout << "accept error: " << ec.message() << std::endl;
            return ;
        }
        auto session = std::make_shared<Session>(std::move(socket), dispatcher);

        session->set_callback([this](std::shared_ptr<Session> session) {
            match_queue_.leave(session->player_id());
            player_manager_.remove_player(session->player_id());

            // Room
            room_manager_.remove_player(session->player_id());
        });

        session->start();

        do_accept(acceptor, dispatcher);
    });
}

void GameServer::start_tick() {
    timer_.expires_after(std::chrono::milliseconds(100));

    timer_.async_wait([this](boost::system::error_code ec) {
        if (ec) {
            return ;
        }

        room_manager_.tick_all();
        start_tick();
    });

}

void GameServer::print_stat() {
    print_timer_.expires_after(std::chrono::milliseconds(1000));

    print_timer_.async_wait([this](boost::system::error_code ec) {
        if (ec) {
            return ;
        }
        std::cout << "[ServerStats] online=" << player_manager_.online_count()
        << " match_queue=" << match_queue_.size() << " room=" << room_manager_.room_count() << std::endl;

        print_stat();
    });

}
