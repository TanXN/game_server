//
// Created by xntan on 2026/6/5.
//

#include "Session.h"

void Session::do_read() {

}

void Session::do_write() {

}

Session::Session(tcp::socket socket, MessageDispatcher &dispatcher) {

}

int Session::player_id() const {
    return player_id_;
}

void Session::close() {

}

void Session::start() {

}

void Session::on_read(const boost::system::error_code &ec, std::size_t length) {

}

void Session::send(const Message &msg) {

}

void Session::ser_player_id(int player_id) {
    player_id_ = player_id;
}
