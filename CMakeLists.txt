cmake_minimum_required(VERSION 3.6)
project(linux-mqtt-sn-gateway)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "./cmake/modules/")

#set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 14)

if(NOT DEFINED TRANSMISSION_PROTOCOL)
    message(FATAL_ERROR "No TRANSMISSION_PROTOCOL defined")
endif()

if (${TRANSMISSION_PROTOCOL} STREQUAL "UDP")
    MESSAGE("Transmission Protocol: Udp")
    add_definitions(-DGATEWAY_TRANSMISSION_PROTOCOL_UDP)
    set(BUILD_LINUX_UDP ON)
elseif (${TRANSMISSION_PROTOCOL} STREQUAL "SERIAL")
    MESSAGE("Transmission Protocol: Serial")
    add_definitions(-DGATEWAY_TRANSMISSION_PROTOCOL_SERIAL)
    set(BUILD_LINUX_SERIAL ON)
elseif (${TRANSMISSION_PROTOCOL} STREQUAL "RH_SERIAL")
    MESSAGE("Transmission Protocol: RH_Serial")
    add_definitions(-DGATEWAY_TRANSMISSION_PROTOCOL_RH_SERIAL)
    set(BUILD_LINUX_RH_SERIAL ON)
elseif (${TRANSMISSION_PROTOCOL} STREQUAL "TELNET")
    MESSAGE("Transmission Protocol: Telnet")
    add_definitions(-DGATEWAY_TRANSMISSION_PROTOCOL_TELNET)
    set(BUILD_LINUX_SERIAL ON)
elseif (${TRANSMISSION_PROTOCOL} STREQUAL "RPI_LORA")
    MESSAGE("Transmission Protocol: Raspberry Pi LoRa")
    add_definitions(-DGATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_RF95)
    set(BUILD_RPI_RF95 ON)
    if(DEFINED OWN_ADDRESS)
        MESSAGE("LoRa Address: ${OWN_ADDRESS}")
        add_definitions(-DOWN_ADDRESS=${OWN_ADDRESS})
    else()
        message(FATAL_ERROR "No OWN_ADDRESS defined")
    endif()
elseif (${TRANSMISSION_PROTOCOL} STREQUAL "RPI_NRF24")
    MESSAGE("Transmission Protocol: Raspberry Pi nRF24")
    add_definitions(-DGATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_NRF24)
    set(BUILD_RPI_NRF24 OFF)
elseif (${TRANSMISSION_PROTOCOL} STREQUAL "SINGLE_TCP")
    MESSAGE("Transmission Protocol: Single TCP")
    add_definitions(-DGATEWAY_TRANSMISSION_PROTOCOL_SINGLE_TCP)
    set(BUILD_LINUX_SINGLE_TCP ON)
endif ()


include_directories(src/core-mqtt-sn-gateway/src)

include_directories(src/arduino-linux-abstraction/src)

include_directories(src/Implementation/paho/pahomqttembeddedc/MQTTClient/src/linux)
include_directories(src/Implementation/paho/pahomqttembeddedc/MQTTClient/src/)
include_directories(src/Implementation/paho/pahomqttembeddedc/MQTTPacket/src)

include_directories(src/Implementation)
include_directories(src/main)

set(IMPLEMENTATIONL_FILES
        src/Implementation/LinuxSystem.cpp
        src/Implementation/LinuxSystem.h
        src/Implementation/LinuxLogger.cpp
        src/Implementation/LinuxLogger.h
        src/Implementation/LinuxPersistent.cpp
        src/Implementation/LinuxPersistent.h
        src/Implementation/LinuxGateway.cpp
        src/Implementation/LinuxGateway.h)

if (${TRANSMISSION_PROTOCOL} STREQUAL "UDP")
    include_directories(src/Implementation/Udp)
    set(IMPLEMENTATIONL_FILES ${IMPLEMENTATIONL_FILES}
            src/Implementation/Udp/LinuxUdpSocket.cpp
            src/Implementation/Udp/LinuxUdpSocket.h)
endif ()

if (${TRANSMISSION_PROTOCOL} STREQUAL "SERIAL")
    include_directories(src/Implementation/Serial)
    set(IMPLEMENTATIONL_FILES ${IMPLEMENTATIONL_FILES}
            src/Implementation/Serial/LinuxSerialSocket.cpp
            src/Implementation/Serial/LinuxSerialSocket.h)
endif ()


if (${TRANSMISSION_PROTOCOL} STREQUAL "RH_SERIAL")
    include_directories(src/Implementation/Serial)
    include_directories(src/Implementation/mqtt-sn-sockets/RadioHeadRpi)    
    include_directories(src/Implementation/mqtt-sn-sockets/RadioHeadRpi/RHutil)    
    set(IMPLEMENTATIONL_FILES ${IMPLEMENTATIONL_FILES}
            src/Implementation/Serial/RHSerialSocket.cpp
            src/Implementation/Serial/RHSerialSocket.h)
endif ()


if (${TRANSMISSION_PROTOCOL} STREQUAL "TELNET")
    include_directories(src/Implementation/Telnet)
    set(IMPLEMENTATIONL_FILES ${IMPLEMENTATIONL_FILES}
            src/Implementation/Telnet/LinuxTelnetSocket.cpp
            src/Implementation/Telnet/LinuxTelnetSocket.h)
endif ()

if (${TRANSMISSION_PROTOCOL} STREQUAL "SINGLE_TCP")
    include_directories(src/Implementation/Tcp)
    set(IMPLEMENTATIONL_FILES ${IMPLEMENTATIONL_FILES}
            src/Implementation/Tcp/SingleTcpSocket.cpp
            src/Implementation/Tcp/SingleTcpSocket.h)
endif ()

if (${BUILD_BLE})
    find_package(PkgConfig REQUIRED)

    # thank you: https://stackoverflow.com/questions/38515316/how-to-configure-dbus-dependencies-with-cmake
    set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
    find_package(DBus REQUIRED)
    include_directories(${DBUS_INCLUDE_DIRS})

    pkg_search_module(GLIB REQUIRED glib-2.0)
    include_directories(${GLIB_INCLUDE_DIRS})

    include_directories(src/Implementation/Ble/SimpleBluetoothLowEnergySocket/libBLE/)
endif (${BUILD_BLE})


add_subdirectory(src/core-mqtt-sn-gateway)
add_subdirectory(src/arduino-linux-abstraction)
#add_subdirectory(src/Implementation/Ble_old)
add_subdirectory(src/Implementation/paho)
if (${BUILD_BLE})
    add_subdirectory(src/Implementation/Ble)
endif (${BUILD_BLE})
#add_subdirectory(src/Implementation/BluetoothLowEnergy)
add_subdirectory(src/Implementation/Tcp)

if (${BUILD_RPI_NRF24})
    add_subdirectory(src/Implementation/mqtt-sn-sockets)
    include_directories(src/Implementation/mqtt-sn-sockets)
    include_directories(src/Implementation/mqtt-sn-sockets/RadioHead)
    include_directories(src/Implementation/mqtt-sn-sockets/RadioHead/RHutil)
endif (${BUILD_RPI_NRF24})

if (${BUILD_RPI_RF95})
    add_subdirectory(src/Implementation/mqtt-sn-sockets)
    include_directories(src/Implementation/mqtt-sn-sockets)
    include_directories(src/Implementation/mqtt-sn-sockets/RadioHeadRpi)
    include_directories(src/Implementation/mqtt-sn-sockets/RadioHeadRpi/RHutil)
endif (${BUILD_RPI_RF95})


add_library(lib-linux-mqtt-sn-gateway ${IMPLEMENTATIONL_FILES})
target_link_libraries(lib-linux-mqtt-sn-gateway pthread core-mqtt-sn-gateway arduino-linux-abstraction PahoLinuxMqttMessageHandler)
if (${BUILD_BLE})
    target_link_libraries(lib-linux-mqtt-sn-gateway libMqttSNBLESocket libgdbus)
endif (${BUILD_BLE})
if (${BUILD_RPI_NRF24})
    if (NOT ${BCM2835_FOUND})
       message(FATAL_ERROR "bcm2835 library not installed. See https://www.airspayce.com/mikem/bcm2835")
    endif (NOT ${BCM2835_FOUND})
    #target_compile_definitions(lib-linux-mqtt-sn-gateway PRIVATE -DRH_PLATFORM=RH_PLATFORM_RASPI)
    target_compile_definitions(lib-linux-mqtt-sn-gateway PRIVATE -DRASPBERRY_PI -DBCM2835_NO_DELAY_COMPATIBILITY)
    add_definitions(-DRH_PLATFORM=RH_PLATFORM_RASPI)
    target_link_libraries(lib-linux-mqtt-sn-gateway lib-rh-nrf24-socket bcm2835)
endif (${BUILD_RPI_NRF24})

if (${BUILD_RPI_RF95})
    target_compile_definitions(lib-linux-mqtt-sn-gateway PRIVATE -DRH_PLATFORM=RH_PLATFORM_RPI)
    target_compile_definitions(lib-linux-mqtt-sn-gateway PRIVATE -DFREQUENCY=868)
    target_compile_definitions(lib-linux-mqtt-sn-gateway PRIVATE -DMODEM_CONFIG_CHOICE=Bw31_25Cr48Sf512)
    target_link_libraries(lib-linux-mqtt-sn-gateway lib-rh96-socket wiringPi)
endif (${BUILD_RPI_RF95})


if (${BUILD_LINUX_RH_SERIAL})
    find_package(bcm2835)
    if (NOT ${BCM2835_FOUND})
       message(FATAL_ERROR "bcm2835 library not installed. See https://www.airspayce.com/mikem/bcm2835")
    endif (NOT ${BCM2835_FOUND})
    # This is a total hack, but to get RadioHead to compile for RaspberryPi with HardwareSerial, but NOT
    # include the serial simulator twice, we need to pre-define the simulator_h header test.
    add_definitions(-D__POSIX__)
    add_definitions(-Dsimulator_h)
    # end of hack
    target_compile_definitions(lib-linux-mqtt-sn-gateway PRIVATE -DRASPBERRY_PI -DBCM2835_NO_DELAY_COMPATIBILITY)
    add_subdirectory(src/Implementation/mqtt-sn-sockets)
    include_directories(src/Implementation/mqtt-sn-sockets)
    include_directories(src/Implementation/mqtt-sn-sockets/RadioHeadRpi)
    include_directories(src/Implementation/mqtt-sn-sockets/RadioHeadRpi/RHutil)
    target_link_libraries(lib-linux-mqtt-sn-gateway lib-rh-datagram-socket bcm2835)
endif (${BUILD_LINUX_RH_SERIAL})


set(MAIN_FILE
        src/main/main.cpp)

add_executable(linux-mqtt-sn-gateway ${MAIN_FILE})
target_link_libraries(linux-mqtt-sn-gateway lib-linux-mqtt-sn-gateway)
if (${BUILD_RPI_NRF24})
    #target_compile_definitions(linux-mqtt-sn-gateway PRIVATE -DRH_PLATFORM=RH_PLATFORM_RASPI)
    target_link_libraries(linux-mqtt-sn-gateway lib-rh-nrf24-socket bcm2835)
endif (${BUILD_RPI_NRF24})
if (${BUILD_RPI_RF95})
    target_compile_definitions(linux-mqtt-sn-gateway PRIVATE -DRH_PLATFORM=RH_PLATFORM_RPI)
    target_link_libraries(linux-mqtt-sn-gateway lib-rh96-socket wiringPi)
endif (${BUILD_RPI_RF95})
#find_package(PkgConfig REQUIRED)
#pkg_search_module(GLIB REQUIRED glib-2.0)
#include_directories(${GLIB_INCLUDE_DIRS})

target_link_libraries(lib-linux-mqtt-sn-gateway
        core-mqtt-sn-gateway
        arduino-linux-abstraction
        #libBluetoothLowEnergySocket
        PahoLinuxMqttMessageHandler)
