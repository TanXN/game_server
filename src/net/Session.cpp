//
// Created by xntan on 2026/6/5.
//

#include "Session.h"
#include <iostream>
#include <boost/asio.hpp>
#include "MessageCodec.h"

void Session::do_read() {
    auto self = shared_from_this();
    socket_.async_read_some(boost::asio::buffer(read_buffer_),
            [self, this](boost::system::error_code ec, std::size_t length) {
                if (ec) {
                    if (ec == boost::asio::error::eof) {
                        std::cout << "[disconnect] player_id=" << player_id_ << std::endl;
                        close();
                        return ;
                    }
                    std::cout << "session read error " << ec.message() << std::endl;
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
            update_last_active_time(std::chrono::steady_clock::now());
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
    if (closed_ || write_queue_.empty()) {
        return ;
    }
    auto self = shared_from_this();
    socket_.async_write_some(boost::asio::buffer(write_queue_.front()),
        [self, this](boost::system::error_code ec, std::size_t length) {
            if (ec) {
                std::cout << "session write error" << ec.message() << std::endl;
                close();
                return ;
            }
            write_queue_.pop_front();
            if (!write_queue_.empty()) {
                do_write();
            }
        });
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

    std::cout << "[session] closed player_id=" << player_id_ << std::endl;

    if (callback_) {
        callback_(shared_from_this());
    }
}

void Session::start() {
    update_last_active_time(std::chrono::steady_clock::now());
    do_read();
}


void Session::send(const Message &msg) {
    std::vector<char> packet = MessageCodec::encode(msg);
    bool empty = write_queue_.empty();
    write_queue_.push_back(packet);
    if (empty) {
        do_write();
    }

}

void Session::set_player_id(int player_id) {
    player_id_ = player_id;
}

int Session::player_id() const {
    return player_id_;
}

void Session::set_callback(Callback callback) {
    callback_ = std::move(callback);
}

void Session::update_last_active_time(std::chrono::steady_clock::time_point time) {
    last_active_time_ = time;
}


bool Session::check_timeout(std::chrono::seconds timeout) {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - last_active_time_);
    if (duration > timeout) {
        return true;
    }
    return false;
}


std::string Session::get_token() {
    return token_;
}

void Session::set_token(std::string token) {
    token_ = token;
}

