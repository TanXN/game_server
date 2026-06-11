//
// Created by xntan on 2026/6/8.
//

#ifndef GAME_SERVER_HEARBEATSERVICE_H
#define GAME_SERVER_HEARBEATSERVICE_H

#include "net/Session.h"
#include "net/Message.h"
#include <memory>

class HeartbeatService {
public:
    HeartbeatService(ServerMetrics& metrics);
    void handle_heartbeat(std::shared_ptr<Session> session, const Message& message);

private:
    ServerMetrics& metrics_;
};


#endif //GAME_SERVER_HEARBEATSERVICE_H
