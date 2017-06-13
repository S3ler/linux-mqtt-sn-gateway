//
// Created by bele on 11.06.17.
//

#include "BluetoothLowEnergyAdvertise.h"

char *BluetoothLowEnergyAdvertise::getMAC() {
    return this->mac;
}

std::chrono::milliseconds BluetoothLowEnergyAdvertise::getTimestamp() {
    return this->timestamp;
}
const char *BluetoothLowEnergyAdvertise::getName() const {
    return name;
}

BluetoothLowEnergyAdvertise::BluetoothLowEnergyAdvertise(const char *mac, const char *name,
                                                         std::chrono::milliseconds timestamp) {
    memset(this->mac, 0, sizeof(this->mac));
    memset(this->name, 0, sizeof(this->name));

    strcpy(this->mac, mac);
    strcpy(this->name, name);
    this->timestamp = timestamp;
}

