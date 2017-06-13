//
// Created by bele on 11.06.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_BLUETOOTHLOWENERGYMESSAGE_H
#define LINUX_MQTT_SN_GATEWAY_BLUETOOTHLOWENERGYMESSAGE_H


#include <global_defines.h>

class BluetoothLowEnergyMessage {
public:
    BluetoothLowEnergyMessage();

    device_address address;
    uint8_t payload[255];
    uint16_t payload_length;

    void setDeviceAddress(const device_address* address);
    void setPayload(const uint8_t* payload, const uint16_t payload_length);
};


#endif //LINUX_MQTT_SN_GATEWAY_BLUETOOTHLOWENERGYMESSAGE_H
