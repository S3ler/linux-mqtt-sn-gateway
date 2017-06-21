//
// Created by bele on 11.06.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_BLUETOOTHLOWENERGYADVERTISE_H
#define LINUX_MQTT_SN_GATEWAY_BLUETOOTHLOWENERGYADVERTISE_H

#include <chrono>
#include <cstring>

class BluetoothLowEnergyAdvertise {
private:
#define MAC_BYTES  6
    char mac[MAC_BYTES * 4];
    char name[255];
    std::chrono::milliseconds timestamp;

public:
    BluetoothLowEnergyAdvertise(const char *mac, const char *name, const std::chrono::milliseconds timestamp);

    const char *getMAC();

    const std::chrono::milliseconds getTimestamp();

    const char *getName() const;
    // BLE spec says that the device name field may be between 0 and 248 octets
};


#endif //LINUX_MQTT_SN_GATEWAY_BLUETOOTHLOWENERGYADVERTISE_H
