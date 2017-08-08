//
// Created by bele on 11.06.17.
//

#include <cstring>
#include "BluetoothLowEnergyMessage.h"


BluetoothLowEnergyMessage::BluetoothLowEnergyMessage(const device_address *address, const uint8_t *payload,
                                                     const uint16_t payload_length) {
    memcpy(&this->address, address, sizeof(device_address));
    if(payload_length>255){
        // error
        // TODO throw exception or stuff
    }
    memcpy(this->payload, payload, payload_length);
    this->payload_length = payload_length;
}

const device_address &BluetoothLowEnergyMessage::getAddress() const {
    return address;
}

const uint8_t *BluetoothLowEnergyMessage::getPayload() const {
    return payload;
}

uint16_t BluetoothLowEnergyMessage::getPayload_length() const {
    return payload_length;
}
