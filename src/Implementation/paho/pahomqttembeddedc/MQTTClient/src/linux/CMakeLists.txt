
include_directories(..)
set(SOURCE_FILES linux.cpp ../MQTTClient.h ../MQTTLogging.h ../FP.h)
add_library(LinuxMqttClient ${SOURCE_FILES})