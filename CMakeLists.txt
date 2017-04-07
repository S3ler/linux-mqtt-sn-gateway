cmake_minimum_required(VERSION 3.6)
project(linux-mqtt-sn-gateway)

set(CMAKE_CXX_STANDARD 11)

include_directories(src/core-mqtt-sn-gateway/src)
include_directories(src/Implementation)
include_directories(src/main)

set(IMPLEMENTATIONL_FILES
        src/Implementation/MqttBrokerImplementation.cpp
        src/Implementation/MqttBrokerImplementation.h
        src/Implementation/LinuxUdpSocket.cpp
        src/Implementation/LinuxUdpSocket.h
        src/Implementation/LinuxSystem.cpp
        src/Implementation/LinuxSystem.h
        src/Implementation/LinuxLogger.cpp
        src/Implementation/LinuxLogger.h
        src/Implementation/LinuxPersistent.cpp
        src/Implementation/LinuxPersistent.h
        src/main/main.cpp)

add_executable(linux-mqtt-sn-gateway ${IMPLEMENTATIONL_FILES})
add_subdirectory(src/core-mqtt-sn-gateway)
target_link_libraries(linux-mqtt-sn-gateway core-mqtt-sn-gateway)
