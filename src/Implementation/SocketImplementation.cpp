//
// Created by bele on 07.04.17.
//

#include "SocketImplementation.h"

bool SocketImplementation::begin() {
    return false;
}

void SocketImplementation::setLogger(LoggerInterface *logger) {

}

void SocketImplementation::setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) {

}

device_address *SocketImplementation::getBroadcastAddress() {
    return nullptr;
}

device_address *SocketImplementation::getAddress() {
    return nullptr;
}

uint8_t SocketImplementation::getMaximumMessageLength() {
    return 0;
}

bool SocketImplementation::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) {
    return false;
}

bool
SocketImplementation::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) {
    return false;
}

bool SocketImplementation::loop() {
    return false;
}
