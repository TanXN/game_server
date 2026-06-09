//
// Created by xntan on 2026/6/5.
//

#include <boost/asio.hpp>
#include <thread>
#include "ClientSimulator.h"
using boost::asio::ip::tcp;

void client_worker() {
    ClientSimulator clientSimulator;
    clientSimulator.test_reconnect();
}

int main() {
    std::size_t thread_count = 1;
    std::vector<std::thread> threads;
    for (std::size_t i=0;i<thread_count;i++) {
        threads.emplace_back(std::thread(client_worker));
    }
    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}