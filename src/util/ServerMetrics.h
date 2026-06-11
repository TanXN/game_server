//
// Created by xntan on 2026/6/10.
//

#ifndef GAME_SERVER_SERVERMETRICS_H
#define GAME_SERVER_SERVERMETRICS_H

#include <cstdint>
#include <atomic>
class ServerMetrics {
public:
    ServerMetrics() = default;

    // =========================
    // 连接 / 在线状态
    // =========================

    void set_online_players(int count);
    void set_connection_count(int count);
    void set_room_count(int count);
    void set_match_queue_size(int count);

    int online_players() const;
    int connection_count() const;
    int room_count() const;
    int match_queue_size() const;

    // =========================
    // 收发包统计
    // =========================

    void incr_recv_packet();
    void incr_send_packet();

    int64_t total_recv_packets();
    int64_t total_send_packets();

    // =========================
    // 协议请求统计
    // =========================

    void incr_login_req();
    void incr_match_req();
    void incr_chat_req();
    void incr_heartbeat_req();
    void incr_ranking_req();
    void incr_reconnect_req();

    int64_t login_req_count() const;
    int64_t match_req_count() const;
    int64_t chat_req_count() const;
    int64_t heartbeat_req_count() const;
    int64_t ranking_req_count() const;
    int64_t reconnect_req_count() const;

    // =========================
    // 延迟统计
    // =========================

    void record_heartbeat(int64_t rtt_ms);
    int64_t total_heartbeat_rtt_ms() const;
    int64_t heartbeat_rtt_count() const;
    double avg_heartbeat_rtt_ms() const;

    // =========================
    // QPS 统计辅助
    // =========================

    int64_t calc_recv_qps();
    int64_t calc_send_qps();

private:
    // 状态指标
    std::atomic<int> online_players_{0};
    std::atomic<int> connection_count_{0};
    std::atomic<int> room_count_{0};
    std::atomic<int> match_queue_size_{0};

    // 总收发包
    std::atomic<int> total_recv_packets_{0};
    std::atomic<int> total_send_packets_{0};

    // 协议计数
    std::atomic<int> login_req_count_{0};
    std::atomic<int> match_req_count_{0};
    std::atomic<int> chat_req_count_{0};
    std::atomic<int> heartbeat_req_count_{0};
    std::atomic<int> ranking_req_count_{0};
    std::atomic<int> reconnect_req_count_{0};

    // 心跳 RTT
    std::atomic<int64_t> total_heartbeat_rtt_ms_{0};
    std::atomic<int64_t> heartbeat_rtt_count_{0};

    // QPS 辅助
    std::atomic<int64_t> last_recv_packets_{0};
    std::atomic<int64_t> last_send_packets_{0};

};


#endif //GAME_SERVER_SERVERMETRICS_H
