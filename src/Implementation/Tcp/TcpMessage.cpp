//
// Created by bele on 13.06.17.
//

#include "TcpMessage.h"

TcpMessage::TcpMessage() {
    memset(&payload, 0, sizeof(payload));
    memset(&deviceAddress, 0, sizeof(deviceAddress));
    payload_length = 0;
}

void TcpMessage::setPayload(uint8_t* payload, uint16_t payload_length) {
    memcpy(&this->payload, payload, payload_length);
}

void TcpMessage::setDeviceAddress(device_address *address) {
    memcpy(&this->deviceAddress, address, sizeof(deviceAddress));
}
