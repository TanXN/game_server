//
// Created by xntan on 2026/6/5.
//

#include "MessageDispatcher.h"

#include <cstdint>
#include <iostream>

#include "Message.h"


void MessageDispatcher::dispatch(SessionPtr session, const Message &message) {
    if (!handlers_.contains(message.msg_id)) {
        std::cout << "MessageDispatcher::dispatch() no found msg_id:" << message.msg_id << std::endl;
        return;
    }

    Handler handler = handlers_.at(message.msg_id);
    handler(session, message);
}

void MessageDispatcher::register_handler(uint16_t msg_id, MessageDispatcher::Handler handler) {
    handlers_[msg_id] = std::move(handler);
}

