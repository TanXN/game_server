//
// Created by xntan on 2026/6/5.
//

#include "MessageCodec.h"

#include <iostream>

#include "net/Message.h"

uint16_t MessageCodec::read_uint16(const char *data) {
    const uint16_t value = static_cast<uint8_t>(data[0]) | static_cast<uint8_t>(data[1]) << 8;
    return value;
}

uint32_t MessageCodec::read_uint32(const char *data) {
    const uint32_t value = static_cast<uint8_t>(data[0]) | static_cast<uint8_t>(data[1]) << 8 |
        static_cast<uint8_t>(data[2]) << 16 | static_cast<uint8_t>(data[3]) << 24;
    return value;
}

void MessageCodec::write_uint16(char *data, uint16_t value) {
    data[0] = value & 0xff;
    data[1] = value >> 8;
}

void MessageCodec::write_uint32(char *data, uint32_t value) {
    data[0] = value & 0xff;
    data[1] = value >> 8 & 0xff;
    data[2] = value >> 16 & 0xff;
    data[3] = value >> 24 & 0xff;
}

std::vector<char> MessageCodec::encode(const Message &message) {
    return encode(message.msg_id, message.body);
}

std::vector<char> MessageCodec::encode(uint16_t msg_id, const std::string &body) {
    std::size_t size = body.size();
    if (size > MAX_BODY_SIZE) {
        std::cout << "Message too long" << std::endl;
        return std::vector<char>();
    }
    char *header = new char[HEADER_SIZE];
    write_uint32(header, size);
    write_uint16(header+4, msg_id);
    std::vector<char> result(size + HEADER_SIZE);
    std::copy(header, header+HEADER_SIZE, result.begin());
    std::copy(body.begin(), body.end(), result.begin()+HEADER_SIZE);
    delete []header;
    return result;
}

MessageCodec::DecodeStatus MessageCodec::try_decode(std::vector<char> &buffer, Message &out_message, std::string *error_message) {
    if (buffer.size() < HEADER_SIZE) {
        *error_message = "Message too short";
        return DecodeStatus::NeedMore;
    }
    uint32_t body_size = read_uint32(buffer.data());
    uint16_t msg_id = read_uint16(buffer.data()+4);
    if (buffer.size() < body_size + HEADER_SIZE) {
        *error_message = "Message too short";
        return DecodeStatus::NeedMore;
    }
    const long packet_size = HEADER_SIZE + body_size;
    out_message.msg_id = msg_id;
    out_message.body = std::string(buffer.begin()+HEADER_SIZE, buffer.begin()+packet_size);
    buffer.erase(buffer.begin(), buffer.begin()+packet_size);
    return DecodeStatus::Success;
}

