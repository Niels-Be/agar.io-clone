cmake_minimum_required(VERSION 2.6)
project(server)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

set(SOURCE_FILES
    src/Ball.cpp
    src/Ball.h
    src/Network/Client.cpp
    src/Network/Client.h
    src/Element.cpp
    src/Element.h
    src/Food.cpp
    src/Food.h
    src/Gamefield.cpp
    src/Gamefield.h
    src/GlobalDefs.h
    src/main.cpp
    src/MoveableElement.cpp
    src/MoveableElement.h
    src/Obstracle.cpp
    src/Obstracle.h
    src/Network/Packet.cpp
    src/Network/Packet.h
    src/Network/PacketManager.cpp
    src/Network/PacketManager.h
    src/Player.cpp
    src/Player.h
    src/Network/Server.cpp
    src/Network/Server.h
    src/Shoot.cpp
    src/Shoot.h
    src/Vector.cpp
    src/Vector.h
    src/Json/JSON.cpp
    src/Json/JSON.h
    src/Json/JSONValue.cpp
    src/Json/JSONValue.h
    CMakeLists.txt)

include_directories(src)

add_executable(server ${SOURCE_FILES} src/Network/AgarPackets.cpp src/Network/AgarPackets.h src/QuadTree.cpp src/QuadTree.h src/LobbyManager.cpp src/LobbyManager.h src/Item.cpp src/Item.h src/ItemEffect.cpp src/ItemEffect.h)

find_package(Boost COMPONENTS system REQUIRED)
target_link_libraries(server ${Boost_LIBRARIES})
target_link_libraries(server pthread)
