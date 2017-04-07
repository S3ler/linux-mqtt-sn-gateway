//
// Created by bele on 07.04.17.
//

#include "LinuxUdpSocket.h"

bool LinuxUdpSocket::begin() {
    return false;
}

void LinuxUdpSocket::setLogger(LoggerInterface *logger) {

}

void LinuxUdpSocket::setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) {

}

device_address *LinuxUdpSocket::getBroadcastAddress() {
    return nullptr;
}

device_address *LinuxUdpSocket::getAddress() {
    return nullptr;
}

uint8_t LinuxUdpSocket::getMaximumMessageLength() {
    return 0;
}

bool LinuxUdpSocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) {
    return false;
}

bool
LinuxUdpSocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) {
    return false;
}

bool LinuxUdpSocket::loop() {
    return false;
}
