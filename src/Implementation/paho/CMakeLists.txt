

include_directories(pahomqttembeddedc/MQTTClient/src/linux)
include_directories(pahomqttembeddedc/MQTTClient/src/)
include_directories(pahomqttembeddedc/MQTTPacket/src)

file(GLOB PAHO_SOURCE_FILES
        pahomqttembeddedc/MQTTPacket/src/*.c
        pahomqttembeddedc/MQTTPacket/src/*.h
        pahomqttembeddedc/MQTTClient/src/*.cpp
        pahomqttembeddedc/MQTTClient/src/*.h
        pahomqttembeddedc/MQTTClient/src/linux/*.cpp
        pahomqttembeddedc/MQTTClient/src/linux/*.h
        )


set(SOURCE_FILES PahoMqttMessageHandler.h PahoMqttMessageHandler.cpp)
add_library(PahoLinuxMqttMessageHandler ${SOURCE_FILES} ${PAHO_SOURCE_FILES} )

