cmake_minimum_required(VERSION 3.6)
project(example-mqtt-sn-gateway)

set(CMAKE_CXX_STANDARD 11)

include_directories(src/core-mqtt-sn-gateway/src)
include_directories(src/Implementation)
include_directories(src/main)

set(IMPLEMENTATIONL_FILES
        src/Implementation/MqttBrokerImplementation.cpp src/Implementation/MqttBrokerImplementation.h
        src/Implementation/SocketImplementation.cpp
        src/Implementation/SocketImplementation.h
        src/Implementation/SystemImplementation.cpp
        src/Implementation/SystemImplementation.h
        src/Implementation/LoggerImplementation.cpp
        src/Implementation/LoggerImplementation.h
        src/Implementation/PersistentImplementation.cpp
        src/Implementation/PersistentImplementation.h
        src/main/main.cpp)

add_executable(example-mqtt-sn-gateway ${IMPLEMENTATIONL_FILES})
add_subdirectory(src/core-mqtt-sn-gateway)
target_link_libraries(example-mqtt-sn-gateway core-mqtt-sn-gateway)
