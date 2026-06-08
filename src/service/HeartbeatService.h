//
// Created by xntan on 2026/6/8.
//

#ifndef GAME_SERVER_HEARBEATSERVICE_H
#define GAME_SERVER_HEARBEATSERVICE_H

#include "net/Session.h"
#include "net/Message.h"
#inlcude <memory>

class HeartbeatService {
public:
    void handle_heartbeat(std::shared_ptr<Session> session, const Message& message);

private:
};


#endif //GAME_SERVER_HEARBEATSERVICE_H
