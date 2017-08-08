//
// Created by bele on 11.06.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_BLUETOOTHLOWENERGYMESSAGE_H
#define LINUX_MQTT_SN_GATEWAY_BLUETOOTHLOWENERGYMESSAGE_H


#include <global_defines.h>

class BluetoothLowEnergyMessage {

private:
    device_address address;
    uint8_t payload[255];
    uint16_t payload_length;

public:
    BluetoothLowEnergyMessage(const device_address* address, const uint8_t* payload, const uint16_t payload_length);

    const device_address &getAddress() const;

    const uint8_t *getPayload() const;

    uint16_t getPayload_length() const;
};


#endif //LINUX_MQTT_SN_GATEWAY_BLUETOOTHLOWENERGYMESSAGE_H
