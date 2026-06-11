//
// Created by xntan on 2026/6/10.
//

#include "ServerMetrics.h"

double ServerMetrics::avg_heartbeat_rtt_ms() const {
    return static_cast<double>(total_heartbeat_rtt_ms_) / heartbeat_rtt_count_;
}

int64_t ServerMetrics::calc_recv_qps() {
    int current = total_recv_packets_.load(std::memory_order_relaxed);
    int last = last_recv_packets_.exchange(current, std::memory_order_relaxed);
    return current - last;
}

int64_t ServerMetrics::calc_send_qps() {
    int current = total_send_packets_.load(std::memory_order_relaxed);
    int last = last_send_packets_.exchange(current, std::memory_order_relaxed);

    return current - last;
}

int64_t ServerMetrics::chat_req_count() const {
    return chat_req_count_;
}

int ServerMetrics::connection_count() const {
    return connection_count_;
}

int64_t ServerMetrics::heartbeat_req_count() const {
    return heartbeat_req_count_;
}

int64_t ServerMetrics::heartbeat_rtt_count() const {
    return heartbeat_rtt_count_;
}

void ServerMetrics::incr_chat_req() {
    ++chat_req_count_;
}

void ServerMetrics::incr_heartbeat_req() {
    ++heartbeat_req_count_;
}

void ServerMetrics::incr_login_req() {
    ++login_req_count_;
}

void ServerMetrics::incr_match_req() {
    ++match_req_count_;
}

void ServerMetrics::incr_ranking_req() {
    ++ranking_req_count_;
}

void ServerMetrics::incr_reconnect_req() {
    ++reconnect_req_count_;
}

void ServerMetrics::incr_recv_packet() {
    ++total_recv_packets_;
}

void ServerMetrics::incr_send_packet() {
    ++total_send_packets_;
}

int64_t ServerMetrics::login_req_count() const {
    return login_req_count_;
}

int ServerMetrics::match_queue_size() const {
    return match_queue_size_;
}

int64_t ServerMetrics::match_req_count() const {
    return match_req_count_;
}

int ServerMetrics::online_players() const {
    return online_players_;
}

int64_t ServerMetrics::ranking_req_count() const {
    return ranking_req_count_;
}

int64_t ServerMetrics::reconnect_req_count() const {
    return reconnect_req_count_;
}

void ServerMetrics::record_heartbeat(int64_t rtt_ms) {
    if (rtt_ms < 0) {
        return ;
    }

    total_heartbeat_rtt_ms_.fetch_add(rtt_ms, std::memory_order_relaxed);
}

int ServerMetrics::room_count() const {
    return room_count_;
}

void ServerMetrics::set_connection_count(int count) {
    connection_count_ = count;
}

void ServerMetrics::set_match_queue_size(int count) {
    match_queue_size_ = count;
}

void ServerMetrics::set_online_players(int count) {
    online_players_ = count;
}

void ServerMetrics::set_room_count(int count) {
    room_count_ = count;
}

int64_t ServerMetrics::total_heartbeat_rtt_ms() const {
    return total_heartbeat_rtt_ms_;
}

int64_t ServerMetrics::total_recv_packets() {
    return total_recv_packets_;
}

int64_t ServerMetrics::total_send_packets() {
    return total_send_packets_;
}
