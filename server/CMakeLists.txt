cmake_minimum_required(VERSION 3.2)
project(server)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/Ball.cpp
    src/Ball.h
    src/Client.cpp
    src/Client.h
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
    src/Packet.cpp
    src/Packet.h
    src/PacketManager.cpp
    src/PacketManager.h
    src/Player.cpp
    src/Player.h
    src/Server.cpp
    src/Server.h
    src/Shoot.cpp
    src/Shoot.h
    src/SocketIO.cpp
    src/SocketIO.h
    src/Vector.cpp
    src/Vector.h
    CMakeLists.txt)

include_directories(src)
include_directories("/home/niels/.node-gyp/0.12.2/deps/v8/include/")
include_directories("/home/niels/.node-gyp/0.12.2/deps/uv/include/")
include_directories("/home/niels/.node-gyp/0.12.2/src/")

add_executable(server ${SOURCE_FILES})

find_package(Boost COMPONENTS system REQUIRED)
target_link_libraries(server ${Boost_LIBRARIES})