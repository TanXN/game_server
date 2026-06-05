//
// Created by xntan on 2026/6/5.
//

#ifndef GAME_SERVER_MESSAGECODEC_H
#define GAME_SERVER_MESSAGECODEC_H

#include <cstdint>
#include <vector>
#include <string>
#include "Message.h"

class MessageCodec {
public:
    static constexpr std::size_t HEADER_SIZE = 6;
    static constexpr uint32_t MAX_BODY_SIZE = 1024*1024;

    enum class DecodeStatus {
        Success,
        NeedMore,
        Error
    };

public:
    // 编码：Message -> TCP 字节流
    static std::vector<char> encode(const Message& message);
    // 编码：msg_id + body -> TCP字节流
    static std::vector<char> encode(uint16_t msg_id, const std::string& body);

    static DecodeStatus try_decode(
        std::vector<char>& buffer,
        Message& out_message,
        std::string* error_message = nullptr
    );

private:
    static void write_uint32(char* data, uint32_t value);
    static void write_uint16(char* data, uint16_t value);

    static uint32_t read_uint32(const char* data);
    static uint16_t read_uint16(const char* data);
};


#endif //GAME_SERVER_MESSAGECODEC_H
