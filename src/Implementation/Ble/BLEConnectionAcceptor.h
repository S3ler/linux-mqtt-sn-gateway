//
// Created by bele on 09.08.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_BLECONNECTIONACCEPTOR_H
#define LINUX_MQTT_SN_GATEWAY_BLECONNECTIONACCEPTOR_H

#include "BLEDeviceStatistic.h"
#include "BLESocket.h"
#include "BLEConnection.h"

#include <atomic>
#include <thread>

#include <Ble/SimpleBluetoothLowEnergySocket/libBLE/BLEAdapter.h>
#include <Ble/SimpleBluetoothLowEnergySocket/libBLE/BLE.h>
#include <unordered_map>

class BLESocket;

class BLEConnectionAcceptor {
public:
    explicit BLEConnectionAcceptor(BLESocket *bleSocket);

public:
    bool start();
    void stop();
    void loop();
    std::string getAdapterMac();
private:
    BLESocket* bleSocket;
    safe_flag adapterFoundFlag;
    std::atomic_bool stopped;
    std::thread acceptorThread;
    std::shared_ptr<BLEAdapter> currentAdapter= nullptr;

    std::shared_ptr<BLEAdapter> waitForAdapter(BLE &ble);
    std::unordered_map<std::string, std::shared_ptr<BLEDeviceStatistic>> deviceStatistics;

    bool deviceStatisticsContains(std::string mac);

};


#endif //LINUX_MQTT_SN_GATEWAY_BLECONNECTIONACCEPTOR_H
