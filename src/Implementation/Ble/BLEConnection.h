//
// Created by bele on 09.08.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_BLECONNECTION_H
#define LINUX_MQTT_SN_GATEWAY_BLECONNECTION_H

#include "BLEDeviceStatistic.h"
#include "BLESocket.h"

#include <Ble/SimpleBluetoothLowEnergySocket/libBLE/BLENUSConnection.h>
#include <Ble/SimpleBluetoothLowEnergySocket/libBLE/BLEAdapter.h>

#include <thread>

class BLESocket;
// TODO add try catch + adept exceptions

class BLEConnection : public std::enable_shared_from_this<BLEConnection> {
public:
    BLEConnection(BLESocket *bleSocket,
                  const std::shared_ptr<BLEAdapter> &bleAdapter,
                  const std::shared_ptr<BLEDevice> &bleDevice,
                  const std::shared_ptr<BLEDeviceStatistic> bleDeviceStatistic);

    virtual ~BLEConnection();

    void connect();

    void handleConnection(std::shared_ptr<BLEConnection> shared_this);

    void stop();

    void send(std::vector<uint8_t> data);

    const std::string getMac() const;

private:
    BLESocket* bleSocket;
    std::shared_ptr<BLEAdapter> bleAdapter;
    std::shared_ptr<BLEDevice> bleDevice;
    std::shared_ptr<BLEDeviceStatistic> bleDeviceStatistic;

    std::thread* connectionThread;
    std::shared_ptr<BLENUSConnection> nusConnection;
    std::atomic_bool signalInterruptReceived;
    std::shared_ptr<BLENUSConnection> bleNUSConnection;
    std::string bleDeviceMac;

};


#endif //LINUX_MQTT_SN_GATEWAY_BLECONNECTION_H
