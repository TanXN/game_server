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


    ClientSimulator();

    void login();

    void match();

    void do_read();

    void start();

    void chat(std::string text);

private:
    void try_parse();
    void handle_message(Message &msg);



    boost::asio::io_context io_;
    tcp::socket socket_;

    int player_id_;
    int room_id_;


    std::array<char, 4096> read_buffer_{};
    std::vector<char> recv_buffer_;
};


#endif //GAME_SERVER_CLIENTSIMULATOR_H
