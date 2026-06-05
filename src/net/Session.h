//
// Created by xntan on 2026/6/5.
//

#ifndef GAME_SERVER_SESSION_H
#define GAME_SERVER_SESSION_H
#include <deque>
#include <memory>
#include <deque>
#include <boost/asio.hpp>

#include "MessageDispatcher.h"
#include "net/Message.h"
using boost::asio::ip::tcp;



class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, MessageDispatcher& dispatcher);

    void start();
    void send(const Message& msg);
    void close();

    int player_id() const;
    void ser_player_id(int player_id);

private:
    void do_read();

    void do_write();

private:
    tcp::socket socket_;
    int player_id_ = 0;
    MessageDispatcher& dispatcher_;

    std::array<char, 4096> read_buffer_{};
    std::vector<char> recv_buffer_;

    std::deque<std::vector<char>> write_queue_;

    void parse_packet();
    bool closed_ = false;



};


#endif //GAME_SERVER_SESSION_H
