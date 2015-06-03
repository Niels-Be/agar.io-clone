cmake_minimum_required(VERSION 3.2)
project(AgarIO)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories("/home/niels/.node-gyp/0.12.2/deps/v8/include/")
include_directories("/home/niels/.node-gyp/0.12.2/deps/uv/include/")
include_directories("/home/niels/.node-gyp/0.12.2/src/")

set(SOURCE_FILES main.cpp)
add_executable(AgarIO ${SOURCE_FILES} Gamefield.cpp Gamefield.h Element.cpp Element.h MoveableElement.cpp MoveableElement.h Ball.cpp Ball.h Shoot.cpp Shoot.h Obstracle.cpp Obstracle.h Food.cpp Food.h Player.cpp Player.h Vector.cpp Vector.h GlobalDefs.h SocketIO.cpp SocketIO.h)