//
// Created by bele on 10.08.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_BLEMESSAGE_H
#define LINUX_MQTT_SN_GATEWAY_BLEMESSAGE_H

#include <string>
#include <vector>

class BLEMessage {
public:
    BLEMessage(std::string mac, std::vector<uint8_t> data);

    const std::string &getMac() const;

    const std::vector<uint8_t> &getData() const;

private:
    std::string mac;
    std::vector<uint8_t > data;
};


#endif //LINUX_MQTT_SN_GATEWAY_BLEMESSAGE_H
