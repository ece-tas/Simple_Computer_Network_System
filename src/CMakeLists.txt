cmake_minimum_required(VERSION 3.22)
project(Assignment3)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS="-g -Wall")
add_executable(Assignment3 main.cpp Client.cpp ApplicationLayerPacket.cpp NetworkLayerPacket.cpp Packet.cpp PhysicalLayerPacket.cpp TransportLayerPacket.cpp Network.cpp Log.cpp)

