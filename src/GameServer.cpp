//
// Created by xntan on 2026/6/5.
//

#include "GameServer.h"

#include <iostream>
#include "net/MessageId.h"
#include "service/LoginService.h"
#include "room/RoomManager.h"
#include "service/HeartbeatService.h"

GameServer::GameServer()
    :acceptor_(io_, tcp::endpoint(tcp::v4(), 9000)),
    room_manager_(player_manager_),
    login_service_(player_manager_, connection_manager_, player_state_manager_),
    match_service_(match_queue_, room_manager_),
    chat_service_(room_manager_),
    reconnect_service_(player_manager_, room_manager_, player_state_manager_),
    timer_(io_),
    print_timer_(io_),
    heartbeat_timer_(io_),
    check_reconnect_timer_(io_)
{
}

void GameServer::start_timeout_check() {
    heartbeat_timer_.expires_after(std::chrono::seconds(5));

    heartbeat_timer_.async_wait([this](boost::system::error_code ec) {
        if (ec) {
            std::cout << "time_out check error: " << ec.message() << std::endl;
            return ;
        }
        connection_manager_.check_timeout(std::chrono::seconds(15));

        start_timeout_check();

    });

}

void GameServer::start_reconnect_timeout_check() {
    check_reconnect_timer_.expires_after(std::chrono::seconds(5));

    check_reconnect_timer_.async_wait([this](boost::system::error_code ec) {
        if (ec) {
            std::cout << "reconnect timeout check error: " << ec.message() << std::endl;
        }
        check_reconnect_timeout();

        start_reconnect_timeout_check();
    });
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
    dispatcher_.register_handler(MessageId::HeartbeatReq,
        [this](const std::shared_ptr<Session> &session, const Message& message) {
            heartbeat_service_.handle_heartbeat(session, message);
        });
    dispatcher_.register_handler(MessageId::ReconnectReq,
        [this](const std::shared_ptr<Session>& session, const Message& message) {
            reconnect_service_.handle_reconnect(session, message);
        });

    std::cout << "server listen on port 9000" << std::endl;
    do_accept(acceptor_, dispatcher_);
    start_tick();
    start_timeout_check();
    start_reconnect_timeout_check();

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
            connection_manager_.remove_session(session);

            // RoomManager和PlayerStateManager先标记

            player_state_manager_.save_online(session->player_id(), room_manager_.get_room_id_by_player(session->player_id()), session->get_token());
            player_state_manager_.mark_disconnected(session->player_id());

            room_manager_.mark_displayer_disconnected(session->player_id());
            // room_manager_.remove_player(session->player_id());

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
    print_timer_.expires_after(std::chrono::milliseconds(5000));

    print_timer_.async_wait([this](boost::system::error_code ec) {
        if (ec) {
            return ;
        }
        std::cout << "[ServerStats] online=" << player_manager_.online_count()
        << " match_queue=" << match_queue_.size() << " room=" << room_manager_.room_count() << std::endl;

        print_stat();
    });

}

void GameServer::check_reconnect_timeout() {
    auto players = player_state_manager_.get_reconnect_timeout_players(std::chrono::seconds(30));

    for (auto& player_id : players) {
        std::cout << "[reconnect timeout] remove player_id="
                  << player_id
                  << std::endl;
        room_manager_.remove_player(player_id);
        player_state_manager_.remove_player(player_id);
    }

}
