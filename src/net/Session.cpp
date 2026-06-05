//
// Created by xntan on 2026/6/5.
//

#include "Session.h"
#include <iostream>

#include "MessageCodec.h"

void Session::do_read() {
    socket_.async_read_some(boost::asio::buffer(read_buffer_),
            [this](boost::system::error_code ec, std::size_t length) {
                if (ec) {
                    std::cout << "session read error" << ec.message() << std::endl;
                    close();
                    return ;
                }
                recv_buffer_.insert(recv_buffer_.end(), read_buffer_.begin(), read_buffer_.begin()+length);


                parse_packet();

                if (!closed_) {
                    do_read();
                }
        });
}

void Session::parse_packet() {
    while (true) {
        Message msg;
        std::string ec;
        MessageCodec::DecodeStatus status = MessageCodec::try_decode(recv_buffer_, msg, &ec);
        if (status == MessageCodec::DecodeStatus::Success) {
            dispatcher_.dispatch(shared_from_this(), msg);
        }else if(status == MessageCodec::DecodeStatus::NeedMore) {
            break;
        }else {
            close();
            return ;
        }

    }
}

void Session::do_write() {

}

Session::Session(tcp::socket socket, MessageDispatcher &dispatcher)
    : dispatcher_(dispatcher), socket_(std::move(socket)) {}


void Session::close() {
    if (closed_) {
        return ;
    }
    closed_ = true;

    boost::system::error_code ec;
    socket_.shutdown(boost::asio::socket_base::shutdown_both, ec);
    socket_.close();
    std::cout << "session closed" << std::endl;

}

void Session::start() {

    do_read();

}


void Session::send(const Message &msg) {
    std::vector<char> packet = MessageCodec::encode(msg);
    write_queue_.push_back(packet);

}

void Session::ser_player_id(int player_id) {
    player_id_ = player_id;
}
int Session::player_id() const {
    return player_id_;
}