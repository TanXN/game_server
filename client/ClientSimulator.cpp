//
// Created by xntan on 2026/6/5.
//

#include "ClientSimulator.h"
#include <iostream>

#include "MessageCodec.h"
#include "MessageId.h"
#include "net/Message.h"
#include "proto/message.pb.h"

ClientSimulator::ClientSimulator(bool chat)
    :socket_(io_), timer_(io_), enable_chat(chat), chat_timer_(io_)
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
                token_ = resp.token();
                std::cout << "[Login] login success, player_id=" << resp.playerid()
                << " token=" << resp.token()
                << std::endl;
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
        case MessageId::HeartbeatResp: {
            game_server::HeartbeatResp resp;
            resp.ParseFromString(msg.body);
            long send_time_ms = resp.client_time_ms();
            auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
            rtt_ms_ = now - send_time_ms;
            std::cout << "[heartbeat] client=" << resp.client_time_ms()
            << " server=" << resp.server_time_ms() << std::endl;
            break;
        }
        case MessageId::ReconnectResp: {
            game_server::ReconnectResp resp;
            resp.ParseFromString(msg.body);
            std::cout << "[reconnect] player_id=" << resp.player_id()
            << " code=" << resp.code()
            << " room_id=" << resp.room_id()
            << " message=" << resp.message()
            << std::endl;

            if (resp.code() == 0) {
                heart_beat_enable_ = true;
                duration_send();
            }

            break;
        }
        case MessageId::RankingResp: {
            game_server::RankingResp resp;
            resp.ParseFromString(msg.body);
            auto items = resp.items();
            std::cout << "[RankingResp]" << std::endl;
            for (int i=0;i<items.size();i++) {
                auto item = items[i];
                int player_id = item.player_id();
                int score = item.score();
                std::cout << "rank=" << i+1 << " player_id=" << player_id << " score=" << score << std::endl;
            }
            break;
        }
        default: {
            std::cout << "no matching messageId=" << msg.msg_id << std::endl;
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
        // std::cout << "received packet msg_id=" << msg.msg_id << " body_size=" << msg.body.size() << std::endl;
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

void ClientSimulator::rank() {
    game_server::RankingReq req;
    req.set_top_n(10);
    Message msg;
    msg.msg_id = MessageId::RankingReq;
    req.SerializeToString(&msg.body);
    auto packet = MessageCodec::encode(msg);
    boost::asio::write(socket_, boost::asio::buffer(packet));
}

void ClientSimulator::start() {

    login();
    duration_send();

    auto rank_timer = boost::asio::steady_timer(io_);
    rank_timer.expires_after(std::chrono::seconds(20));
    rank_timer.async_wait([this](boost::system::error_code ec) {
        if (ec) {
            std::cout << "rank error : " << ec.message() << std::endl;
            return ;
        }
        rank();
    });

    do_read();


    duration_chat();


    io_.run();

}


void ClientSimulator::send_heartbeat() {
    game_server::HeartbeatReq req;
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    auto mills = duration.count();
    req.set_client_time_ms(mills);
    req.set_last_rtt_ms(rtt_ms_);

    Message msg;
    msg.msg_id = MessageId::HeartbeatReq;
    req.SerializeToString(&msg.body);

    auto packet  = MessageCodec::encode(msg);
    socket_.write_some(boost::asio::buffer(packet));
}


void ClientSimulator::duration_send() {
    timer_.expires_after(std::chrono::seconds(5));
    if (!heart_beat_enable_) {
        return ;
    }
    timer_.async_wait([this](boost::system::error_code ec) {
        if (ec) {
         std::cout << "duration_send error : " << ec.message() << std::endl;
         return ;
        }
        send_heartbeat();

        duration_send();
    });

}

void ClientSimulator::stop_heartbeat() {
    heart_beat_enable_ = false;
    boost::system::error_code ec;
    timer_.cancel(ec);
    std::cout << "client " << player_id_ << " stopped heartbeat" << std::endl;
}

void ClientSimulator::reconnect() {
    std::cout << "[Reconnect] start player_id="
              << player_id_
              << " token="
              << token_
              << std::endl;

    if (player_id_ == 0 || token_ == "") {
        std::cout << "player=0 || token==''" << std::endl;
        return ;
    }
    boost::system::error_code ec;
    socket_.close(ec);
    socket_ = tcp::socket(io_);
    tcp::resolver resolver(io_);
    auto endpoints = resolver.resolve("127.0.0.1", "9000");

    boost::asio::connect(socket_, endpoints);
    std::cout << "[Reconnect] connected to server again" << std::endl;
    recv_buffer_.clear();

    do_read();

    game_server::ReconnectReq req;
    req.set_player_id(player_id_);
    req.set_token(token_);
    Message msg;
    msg.msg_id = MessageId::ReconnectReq;
    req.SerializeToString(&msg.body);

    auto packet = MessageCodec::encode(msg);

    boost::asio::write(socket_, boost::asio::buffer(packet));
    // send_message(MessageId::ReconnectReq, req);



}


void ClientSimulator::test_reconnect() {
    login();
    heart_beat_enable_ = false;
    duration_send();

    auto stop_heartbeat_timer = boost::asio::steady_timer(io_);
    stop_heartbeat_timer.expires_after(std::chrono::seconds(5));
    stop_heartbeat_timer.async_wait([this](boost::system::error_code ec) {
        if (ec) {
            std::cout << "stop_heartbeat_timer error : " << ec.message() << std::endl;
            return ;
        }
        stop_heartbeat();
    });

    auto reconnect_timer = boost::asio::steady_timer(io_);
    reconnect_timer.expires_after(std::chrono::seconds(80));
    reconnect_timer.async_wait([this](boost::system::error_code ec) {
        if (ec) {
            std::cout << "reconnect_timer error : " << ec.message() << std::endl;
            return ;
        }
        reconnect();
    });

    do_read();
    io_.run();

}

void ClientSimulator::duration_chat() {
    if (!enable_chat) {
        return ;
    }
    chat_timer_.expires_after(std::chrono::seconds(2));
    chat_timer_.async_wait([this](boost::system::error_code ec) {
        if (ec) {
            std::cout << "duration_chat error : " << ec.message() << std::endl;
            return ;
        }
        chat("this is a chat");

        duration_chat();
    });

}
