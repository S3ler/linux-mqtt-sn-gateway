//
// Created by bele on 13.06.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_BLACKLISTENTRY_H
#define LINUX_MQTT_SN_GATEWAY_BLACKLISTENTRY_H


#include <chrono>

class BlacklistEntry {
public:
    char mac[19];
    std::chrono::milliseconds timestamp;
};


#endif //LINUX_MQTT_SN_GATEWAY_BLACKLISTENTRY_H
