//
// Created by xntan on 2026/6/5.
//

#ifndef GAME_SERVER_MESSAGE_H
#define GAME_SERVER_MESSAGE_H

#include <cstdint>
#include <string>

struct Message {
    uint16_t msg_id = 0;
    std::string body;
};


#endif //GAME_SERVER_MESSAGE_H
