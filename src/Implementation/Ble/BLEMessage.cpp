//
// Created by bele on 10.08.17.
//

#include "BLEMessage.h"

BLEMessage::BLEMessage(const std::string mac, const std::vector<uint8_t> data) : mac(mac), data(data) {}

const std::string &BLEMessage::getMac() const {
    return mac;
}

const std::vector<uint8_t> &BLEMessage::getData() const {
    return data;
}
