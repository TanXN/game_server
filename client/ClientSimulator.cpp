//
// Created by xntan on 2026/6/5.
//

#include "ClientSimulator.h"
#include <iostream>

#include "MessageCodec.h"
#include "MessageId.h"
#include "net/Message.h"
#include "proto/message.pb.h"

ClientSimulator::ClientSimulator()
    :socket_(io_), timer_(io_)
{
    auto resolver = boost::asio::ip::tcp::resolver(io_);
    auto endpoints = resolver.resolve("127.0.0.1", "9000");
    boost::asio::connect(socket_, endpoints);
    std::cout << "connected to server" << std::endl;
}

void ClientSimulator::login() {
    Message msg;
    game_server::LoginReq req;
    req.set_username("player");

    msg.msg_id = MessageId::LoginReq;
    req.SerializeToString(&msg.body);

    auto packet = MessageCodec::encode(msg);

    boost::asio::write(socket_, boost::asio::buffer(packet));
    std::cout << "send msg_id=" << msg.msg_id
          << " body_size=" << msg.body.size()
          << std::endl;
}

void ClientSimulator::match() {
    Message msg;
    game_server::MatchReq req;
    if (player_id_ <= 0) {
        std::cout << "player_id should > 0 player_id=" << player_id_ << std::endl;
        return ;
    }
    req.set_playerid(player_id_);
    msg.msg_id = MessageId::MatchReq;
    req.SerializeToString(&msg.body);

    auto packet = MessageCodec::encode(msg);

    boost::asio::write(socket_, boost::asio::buffer(packet));
    std::cout << "send match request msg_id=" << msg.msg_id
          << " body_size=" << msg.body.size()
          << std::endl;
}

void ClientSimulator::handle_message(Message& msg) {
    switch (msg.msg_id) {
        case MessageId::LoginResp: {
            game_server::LoginResp resp;
            resp.ParseFromString(msg.body);
            if (resp.code() == 0) {
                player_id_ = resp.playerid();
                std::cout << "[Login] login success, player_id=" << resp.playerid() << std::endl;
                match();
            }
            break;
        }
        case MessageId::MatchResp: {
            game_server::MatchResp resp;
            resp.ParseFromString(msg.body);
            if (resp.code() == 0) {
                const std::string message = resp.message();
                std::cout << "[match] message=" << message << std::endl;
            }
            break;
        }
        case MessageId::RoomCreatedNotify: {
            game_server::RoomCreatedNotify resp;
            resp.ParseFromString(msg.body);
            std::cout << "[RoomCreatedNotify] room_id=" << resp.roomid()
            << " players=";
            room_id_ = resp.roomid();
            auto players = resp.players();
            for (auto p : players) {
                std::cout << p << " ";
            }
            std::cout << std::endl;

            std::string text = "hello from " + std::to_string(player_id_);
            chat(text);
            break;
        }
        case MessageId::ChatNotify: {
            game_server::ChatNotify resp;
            resp.ParseFromString(msg.body);
            std::cout << "[ChatNotify] room_id=" << resp.roomid()
            << " player_id=" << resp.playerid()
            << " text=" << resp.text() << std::endl;
            break;
        }
        default: {
            std::cout << "no matching messageId" << std::endl;
            break;
        }

    }

}

void ClientSimulator::try_parse() {

    while (true) {
        std::string error_msg;
        Message msg;
        if (MessageCodec::try_decode(recv_buffer_, msg, &error_msg) != MessageCodec::DecodeStatus::Success) {
            break;
        }
        std::cout << "received packet msg_id=" << msg.msg_id << " body_size=" << msg.body.size() << std::endl;
        handle_message(msg);

    }

}

void ClientSimulator::do_read() {
    socket_.async_read_some(boost::asio::buffer(read_buffer_),
        [this](boost::system::error_code ec, std::size_t length) {
            if (ec) {
                std::cout << "read error : " << ec.message() << std::endl;
                return ;
            }

            recv_buffer_.insert(recv_buffer_.end(), read_buffer_.begin(), read_buffer_.begin()+length);

            try_parse();

            do_read();
        }
    );
}

void ClientSimulator::chat(std::string text) {
    game_server::ChatReq req;
    req.set_roomid(room_id_);
    req.set_text(text);
    Message msg;
    msg.msg_id = MessageId::ChatReq;
    req.SerializeToString(&msg.body);
    auto packet = MessageCodec::encode(msg);
    boost::asio::write(socket_, boost::asio::buffer(packet));
}

void ClientSimulator::start() {

    login();

    do_read();
    io_.run();

    send_heartbeat();
}


void ClientSimulator::send_heartbeat() {
    game_server::HeartbeatReq req;
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    auto mills = duration.count();
    req.set_client_time_ms(mills);

    Message msg;
    msg.msg_id = MessageId::HeartbeatReq;
    req.SerializeToString(&msg.body);

    auto packet  = MessageCodec::encode(msg);

    socket_.write_some(boost::asio::buffer(packet));
}
