//
// Created by bele on 11.06.17.
//

#include "BluetoothLowEnergyAdvertise.h"

const char *BluetoothLowEnergyAdvertise::getMAC() {
    return this->mac;
}

const std::chrono::milliseconds BluetoothLowEnergyAdvertise::getTimestamp() {
    return this->timestamp;
}

const char *BluetoothLowEnergyAdvertise::getName() const {
    return name;
}

BluetoothLowEnergyAdvertise::BluetoothLowEnergyAdvertise(const char *mac, const char *name,
                                                         const std::chrono::milliseconds timestamp) {
    memset(this->mac, 0, sizeof(this->mac));
    memset(this->name, 0, sizeof(this->name));

    strcpy((char *) this->mac, mac);
    if (name != NULL) {
        strcpy(this->name, name);
    }
    this->timestamp = timestamp;
}

