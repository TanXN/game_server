//
// Created by xntan on 2026/6/5.
//

#ifndef GAME_SERVER_MESSAGEDISPATCHER_H
#define GAME_SERVER_MESSAGEDISPATCHER_H

#include <functional>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include "Message.h"

class Session;


class MessageDispatcher {
public:
    using SessionPtr = std::shared_ptr<Session>;
    using Handler = std::function<void(SessionPtr, const Message&)>;

public:
    void register_handler(uint16_t msg_id, Handler handler);
    void dispatch(SessionPtr session, const Message& message);

private:
    std::unordered_map<uint16_t, Handler> handlers_;
};


#endif //GAME_SERVER_MESSAGEDISPATCHER_H
