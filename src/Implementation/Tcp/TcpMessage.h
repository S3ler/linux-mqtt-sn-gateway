//
// Created by bele on 13.06.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_TCPMESSAGE_H
#define LINUX_MQTT_SN_GATEWAY_TCPMESSAGE_H

#include <global_defines.h>
#include <cstdint>
#include <cstring>


class TcpMessage {
    uint8_t payload[255];
    uint16_t payload_length=0;
    device_address deviceAddress;

public:
    TcpMessage();

    void setPayload(uint8_t* payload, uint16_t payload_length);

    void setDeviceAddress(device_address *address);
};


#endif //LINUX_MQTT_SN_GATEWAY_TCPMESSAGE_H
