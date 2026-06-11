//
// Created by xntan on 2026/6/8.
//

#include "HeartbeatService.h"

#include "MessageId.h"
#include "proto/message.pb.h"
#include "net/Session.h"
#include "net/Message.h"

HeartbeatService::HeartbeatService(ServerMetrics &metrics)
:metrics_(metrics)
{

}


void HeartbeatService::handle_heartbeat(std::shared_ptr<Session> session, const Message& message) {
    metrics_.incr_heartbeat_req();
    game_server::HeartbeatReq req;
    req.ParseFromString(message.body);

    auto now = std::chrono::steady_clock::now();

    // 记录rtt时间
    long rtt = req.last_rtt_ms();
    if (rtt > 0) {
        metrics_.record_heartbeat(rtt);
    }

    session->update_last_active_time(now);

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    // std::cout << "[heartbeat] recv from player " << session->player_id()
    // << " time=" << req.client_time_ms()
    // << std::endl;

    Message msg;
    msg.msg_id = MessageId::HeartbeatResp;
    game_server::HeartbeatResp resp;
    resp.set_client_time_ms(req.client_time_ms());
    resp.set_server_time_ms(duration.count());
    resp.SerializeToString(&msg.body);
    session->send(msg);
}
