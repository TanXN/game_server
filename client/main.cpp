//
// Created by xntan on 2026/6/5.
//
#include <iostream>
#include <vector>
#include <array>
#include <string>

#include <boost/asio.hpp>
#include "../src/net/MessageId.h"
#include "../src/net/Message.h"
#include "MessageCodec.h"
#include "proto/message.pb.h"
#include "ClientSimulator.h"
using boost::asio::ip::tcp;

int main() {
    ClientSimulator clientSimulator;
    clientSimulator.start();
}