//
// Created by bele on 11.06.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_PERPHERALCONNECTION_H
#define LINUX_MQTT_SN_GATEWAY_PERPHERALCONNECTION_H


#include <global_defines.h>
#include <BluetoothLowEnergy/gattlib/include/gattlib.h>
#include "PeripherapConnectionCreator.h"
#include "LinuxBluetoothLowEnergySocket.h"

class PeripherapConnectionCreator;
class LinuxBluetoothLowEnergySocket;

class PerpheralConnection {
public:
    uint16_t tx_handle = 0, rx_handle = 0;
    int rx_cccd_handle = -1;

    bool stopped;
    device_address address;
    std::thread thread;
    device_address* getAddress();
    gatt_connection_t* m_connection = NULL;

    bool send(uint8_t* payload, uint16_t payload_length);
    bool connect(const char *mac);
    void run();
    void loop();
    void stop();

    const char* getMAC();

    void setPeripheralCreator(PeripherapConnectionCreator *pCreator);

    void setAdvertise(BluetoothLowEnergyAdvertise *pAdvertise);

    PeripherapConnectionCreator *creater;
    BluetoothLowEnergyAdvertise *advertise = nullptr;

    void setBLESocket(LinuxBluetoothLowEnergySocket *bleSocket);

    LinuxBluetoothLowEnergySocket *bleSocket;

    void receiveData(const uint8_t *data, size_t data_length);

    ~PerpheralConnection();

    volatile bool error = false;
};


#endif //LINUX_MQTT_SN_GATEWAY_PERPHERALCONNECTION_H
