//
// Created by bele on 11.06.17.
//

#include <cstring>
#include "BluetoothLowEnergyMessage.h"

BluetoothLowEnergyMessage::BluetoothLowEnergyMessage() {
    memset(&this->address, 0, sizeof(device_address));
    memset(this->payload, 0, sizeof(payload));
    payload_length = 0;
}

void BluetoothLowEnergyMessage::setDeviceAddress(const device_address* address) {
    memcpy(&this->address, address, sizeof(device_address));
}

void BluetoothLowEnergyMessage::setPayload(const uint8_t *payload, const uint16_t payload_length) {
    if(payload_length>255){
        // error
        // TODO throw exception or stuff
    }
    memcpy(this->payload, payload, payload_length);
    this->payload_length = payload_length;
}
