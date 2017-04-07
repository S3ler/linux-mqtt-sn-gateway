set(SOURCE_FILES hello.cpp)
set(INCLUDE_DIRECTORIES ../../src ../../src/linux ../../../MQTTPacket/src)
include_directories(${INCLUDE_DIRECTORIES})

add_executable(runPahoLinuxMqttClient_Hello ${SOURCE_FILES})
target_link_libraries(runPahoLinuxMqttClient_Hello MQTTPacket LinuxMqttClient)