//
// Created by xntan on 2026/6/5.
//

#ifndef GAME_SERVER_CLIENTSIMULATOR_H
#define GAME_SERVER_CLIENTSIMULATOR_H

#include <boost/asio.hpp>
#include <array>
#include <google/protobuf/message.h>

#include "net/Message.h"
using boost::asio::ip::tcp;

class ClientSimulator {
public:


    ClientSimulator(bool chat = false);

    void login();

    void match();

    void do_read();

    void start();

    void chat(std::string text);
    void stop_heartbeat();

    void test_reconnect();

private:
    void try_parse();
    void handle_message(Message &msg);
    void send_heartbeat();
    void duration_send();
    void reconnect();
    void duration_chat();
    void rank();


    boost::asio::io_context io_;
    tcp::socket socket_;

    int player_id_;
    int room_id_;
    bool heart_beat_enable_ = true;
    bool enable_chat = false;
    long rtt_ms_ = -1;
    std::string token_;

    std::array<char, 4096> read_buffer_{};
    std::vector<char> recv_buffer_;

    boost::asio::steady_timer timer_;
    boost::asio::steady_timer chat_timer_;
};


#endif //GAME_SERVER_CLIENTSIMULATOR_H
