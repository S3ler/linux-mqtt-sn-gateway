//
// Created by bele on 11.06.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_SCANNER_H
#define LINUX_MQTT_SN_GATEWAY_SCANNER_H


#include "Queue.h"
#include "BluetoothLowEnergyAdvertise.h"
#include <thread>

#define BLE_SCAN_TIMEOUT   10


// TODO adapter not threadsafe
// TODO static class member init

class Scanner {
private:
    static Queue<BluetoothLowEnergyAdvertise *> *queue;
    void *adapter = nullptr;

public:
    bool scan_enable();

    void scan_disable();

    void setAdapter(void *adapter);

    void setScannerQueue(Queue<BluetoothLowEnergyAdvertise *> *queue);

    ~Scanner();

private:
    bool scan_enable(void *adapter);

    static void ble_discovered_device(const char *addr, const char *name);

};

#endif //LINUX_MQTT_SN_GATEWAY_SCANNER_H
