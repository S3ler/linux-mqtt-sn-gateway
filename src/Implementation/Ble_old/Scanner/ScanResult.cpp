//
// Created by bele on 09.07.17.
//

#include <cstring>
#include "ScanResult.h"

device_address *ScanResult::getDeviceAddress() const {
    return (device_address *) &address;
}

ScanResult::ScanResult(const device_address *address) {
    memcpy(this->address.bytes, address->bytes, sizeof(device_address));
}
