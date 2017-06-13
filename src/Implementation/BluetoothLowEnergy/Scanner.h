//
// Created by bele on 11.06.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_SCANNER_H
#define LINUX_MQTT_SN_GATEWAY_SCANNER_H


#include "Queue.h"
#include "BluetoothLowEnergyAdvertise.h"
#include <thread>

#define BLE_SCAN_TIMEOUT   10

class Scanner {
public:
    static Queue<BluetoothLowEnergyAdvertise *> *queue ;

public:
    void start_loop();

    void loop();

    void stop_loop();

    bool stopped;
    std::thread thread;

    void setScannerQueue(Queue<BluetoothLowEnergyAdvertise *> *queue);


    bool scan_enable(void *adapter);

    void *adapter;

    static void ble_discovered_device(const char *addr, const char *name);

    ~Scanner();
};

/*
#ifndef LINUX_MQTT_SN_GATEWAY_SCANNER_STATIC_INIT
#define LINUX_MQTT_SN_GATEWAY_SCANNER_STATIC_INIT
    Queue<BluetoothLowEnergyAdvertise *> *Scanner::queue = nullptr;
#endif
*/
#endif //LINUX_MQTT_SN_GATEWAY_SCANNER_H
