cmake_minimum_required(VERSION 3.6)
project(linux-mqtt-sn-gateway)

set(CMAKE_CXX_STANDARD 11)

include_directories(src/core-mqtt-sn-gateway/src)

include_directories(src/arduino-linux-abstraction/src)

include_directories(src/Implementation/paho/pahomqttembeddedc/MQTTClient/src/linux)
include_directories(src/Implementation/paho/pahomqttembeddedc/MQTTClient/src/)
include_directories(src/Implementation/paho/pahomqttembeddedc/MQTTPacket/src)

include_directories(src/Implementation)
include_directories(src/main)

set(IMPLEMENTATIONL_FILES
        src/Implementation/Udp/LinuxUdpSocket.cpp
        src/Implementation/Udp/LinuxUdpSocket.h
        src/Implementation/LinuxSystem.cpp
        src/Implementation/LinuxSystem.h
        src/Implementation/LinuxLogger.cpp
        src/Implementation/LinuxLogger.h
        src/Implementation/LinuxPersistent.cpp
        src/Implementation/LinuxPersistent.h
        src/Implementation/LinuxGateway.cpp
        src/Implementation/LinuxGateway.h)

set(MAIN_FILE
        src/main/main.cpp)

add_executable(linux-mqtt-sn-gateway ${IMPLEMENTATIONL_FILES} ${MAIN_FILE})



add_subdirectory(src/core-mqtt-sn-gateway)
add_subdirectory(src/arduino-linux-abstraction)
add_subdirectory(src/Implementation/paho)
add_subdirectory(src/Implementation/BluetoothLowEnergy)

target_link_libraries(linux-mqtt-sn-gateway pthread core-mqtt-sn-gateway arduino-linux-abstraction PahoLinuxMqttMessageHandler)

add_library(lib-linux-mqtt-sn-gateway  ${IMPLEMENTATIONL_FILES} )
target_link_libraries(lib-linux-mqtt-sn-gateway core-mqtt-sn-gateway arduino-linux-abstraction PahoLinuxMqttMessageHandler)