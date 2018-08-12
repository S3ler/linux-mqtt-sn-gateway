cmake_minimum_required(VERSION 3.6)
project(linux-mqtt-sn-gateway)

set(CMAKE_CXX_STANDARD 14)

set(BUILD_BLE OFF)

set(BUILD_RPI OFF)
if(${BUILD_RPI})
add_definitions(-DGATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_NRF24)
endif(${BUILD_RPI})

set(BUILD_LORA OFF)
if(${BUILD_LORA})
add_definitions(-DGATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_RF95)
endif(${BUILD_LORA})
add_definitions(-DGATEWAY_TRANSMISSION_PROTOCOL_UDP)

include_directories(src/core-mqtt-sn-gateway/src)

include_directories(src/arduino-linux-abstraction/src)

include_directories(src/Implementation/paho/pahomqttembeddedc/MQTTClient/src/linux)
include_directories(src/Implementation/paho/pahomqttembeddedc/MQTTClient/src/)
include_directories(src/Implementation/paho/pahomqttembeddedc/MQTTPacket/src)

include_directories(src/Implementation)
include_directories(src/main)

set(IMPLEMENTATIONL_FILES
        src/Implementation/Serial/LinuxSerialSocket.h
        src/Implementation/Serial/LinuxSerialSocket.cpp
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


if(${BUILD_BLE})
find_package(PkgConfig REQUIRED)

# thank you: https://stackoverflow.com/questions/38515316/how-to-configure-dbus-dependencies-with-cmake
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
find_package(DBus REQUIRED)
include_directories(${DBUS_INCLUDE_DIRS})

pkg_search_module(GLIB REQUIRED glib-2.0)
include_directories(${GLIB_INCLUDE_DIRS})

include_directories(src/Implementation/Ble/SimpleBluetoothLowEnergySocket/libBLE/)
endif(${BUILD_BLE})


add_subdirectory(src/core-mqtt-sn-gateway)
add_subdirectory(src/arduino-linux-abstraction)
#add_subdirectory(src/Implementation/Ble_old)
add_subdirectory(src/Implementation/paho)
if(${BUILD_BLE})
add_subdirectory(src/Implementation/Ble)
endif(${BUILD_BLE})
#add_subdirectory(src/Implementation/BluetoothLowEnergy)
add_subdirectory(src/Implementation/Tcp)

if(${BUILD_RPI})
add_subdirectory(src/Implementation/mqtt-sn-sockets)
include_directories(src/Implementation/mqtt-sn-sockets)
include_directories(src/Implementation/mqtt-sn-sockets/RadioHead)
include_directories(src/Implementation/mqtt-sn-sockets/RadioHead/RHutil)
endif(${BUILD_RPI})

if(${BUILD_LORA})
add_subdirectory(src/Implementation/mqtt-sn-sockets)
include_directories(src/Implementation/mqtt-sn-sockets)
include_directories(src/Implementation/mqtt-sn-sockets/RadioHeadRpi)
include_directories(src/Implementation/mqtt-sn-sockets/RadioHeadRpi/RHutil)
endif(${BUILD_LORA})


add_library(lib-linux-mqtt-sn-gateway ${IMPLEMENTATIONL_FILES})
target_link_libraries(lib-linux-mqtt-sn-gateway pthread core-mqtt-sn-gateway arduino-linux-abstraction PahoLinuxMqttMessageHandler)
if(${BUILD_BLE})
target_link_libraries(lib-linux-mqtt-sn-gateway libMqttSNBLESocket libgdbus)
endif(${BUILD_BLE})
if(${BUILD_RPI})
#target_compile_definitions(lib-linux-mqtt-sn-gateway PRIVATE -DRH_PLATFORM=RH_PLATFORM_RASPI)
target_compile_definitions(lib-linux-mqtt-sn-gateway PRIVATE -DRASPBERRY_PI -DBCM2835_NO_DELAY_COMPATIBILITY)
add_definitions(-DRH_PLATFORM=RH_PLATFORM_RASPI)
target_link_libraries(lib-linux-mqtt-sn-gateway lib-rh-nrf24-socket bcm2835)
endif(${BUILD_RPI})

if(${BUILD_LORA})
target_compile_definitions(lib-linux-mqtt-sn-gateway PRIVATE -DRH_PLATFORM=RH_PLATFORM_RPI)
target_compile_definitions(lib-linux-mqtt-sn-gateway PRIVATE -DFREQUENCY=868)
target_compile_definitions(lib-linux-mqtt-sn-gateway PRIVATE -DMODEM_CONFIG_CHOICE=Bw31_25Cr48Sf512)
target_link_libraries(lib-linux-mqtt-sn-gateway lib-rh96-socket wiringPi)
endif(${BUILD_LORA})


set(MAIN_FILE
        src/main/main.cpp)

add_executable(linux-mqtt-sn-gateway ${MAIN_FILE})
target_link_libraries(linux-mqtt-sn-gateway lib-linux-mqtt-sn-gateway)
if(${BUILD_RPI})
#target_compile_definitions(linux-mqtt-sn-gateway PRIVATE -DRH_PLATFORM=RH_PLATFORM_RASPI)
target_link_libraries(linux-mqtt-sn-gateway lib-rh-nrf24-socket bcm2835)
endif(${BUILD_RPI})
if(${BUILD_LORA})
target_compile_definitions(linux-mqtt-sn-gateway PRIVATE -DRH_PLATFORM=RH_PLATFORM_RPI)
target_link_libraries(linux-mqtt-sn-gateway lib-rh96-socket wiringPi)
endif(${BUILD_LORA})
#find_package(PkgConfig REQUIRED)
#pkg_search_module(GLIB REQUIRED glib-2.0)
#include_directories(${GLIB_INCLUDE_DIRS})

target_link_libraries(lib-linux-mqtt-sn-gateway
        core-mqtt-sn-gateway
        arduino-linux-abstraction
        #libBluetoothLowEnergySocket
        PahoLinuxMqttMessageHandler)
