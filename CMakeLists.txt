cmake_minimum_required(VERSION 3.28)
project(game_server)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED COMPONENTS system)
find_package(Protobuf REQUIRED)

add_executable(game_server
        src/main.cpp
)

target_include_directories(
        game_server
        PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/net

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
        src/GameServer.cpp
        src/GameServer.cpp
)

target_link_libraries(
        game_client
        PRIVATE
        Boost::system
        protobuf::libprotobuf
        pthread
)