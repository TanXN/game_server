cmake_minimum_required(VERSION 3.28)
project(game_server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost REQUIRED COMPONENTS system)
find_package(Protobuf REQUIRED)

add_executable(game_server
        src/main.cpp
        src/net/Session.cpp
        src/net/MessageDispatcher.cpp
        src/net/MessageCodec.cpp
        generated/proto/message.pb.cc
        src/GameServer.cpp
        src/match/MatchQueue.cpp
        src/service/MatchService.cpp
        src/service/LoginService.cpp
        src/player/PlayerManager.cpp
        src/service/MatchService.cpp
        src/room/RoomManager.cpp
        src/room/Room.cpp
        src/service/ChatService.cpp
        src/service/HeartbeatService.cpp
        src/net/ConnectionManager.cpp
        src/player/PlayerRuntimeState.h
        src/player/PlayerStateManager.cpp
        src/service/ReconnectService.cpp
        src/service/RankingService.cpp
        src/player/MemoryPlayerStateRepository.cpp
        src/player/PlayerStateRepository.h
        src/util/MetricsReporter.cpp
        src/util/ServerMetrics.cpp
)

target_include_directories(
        game_server
        PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/net
        ${CMAKE_SOURCE_DIR}/generated
        ${CMAKE_SOURCE_DIR}/util


)

target_link_libraries(
        game_server
        PRIVATE
        Boost::system
        protobuf::libprotobuf
        pthread
)


add_executable(
        game_client
        client/main.cpp
        src/net/MessageCodec.cpp

        client/ClientSimulator.cpp
        generated/proto/message.pb.cc


)

target_include_directories(
        game_client
        PRIVATE
        src/net
        src/
        generated/
)
target_link_libraries(
        game_client
        PRIVATE
        Boost::system
        protobuf::libprotobuf
        pthread
)