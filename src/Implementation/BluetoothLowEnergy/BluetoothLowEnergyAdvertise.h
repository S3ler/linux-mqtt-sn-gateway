//
// Created by bele on 11.06.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_BLUETOOTHLOWENERGYADVERTISE_H
#define LINUX_MQTT_SN_GATEWAY_BLUETOOTHLOWENERGYADVERTISE_H

#include <chrono>
#include <cstring>

#define MAC_LENGTH  6

class BluetoothLowEnergyAdvertise {

public:
    BluetoothLowEnergyAdvertise(const char *mac, const char *name, std::chrono::milliseconds timestamp);

    char *getMAC();
    std::chrono::milliseconds getTimestamp();
    char mac[19];//TODO check format!
    std::chrono::milliseconds timestamp;
    char name[255];

    const char *getName() const;
    // BLE spec says that the device name field may be between 0 and 248 octets
};


#endif //LINUX_MQTT_SN_GATEWAY_BLUETOOTHLOWENERGYADVERTISE_H
