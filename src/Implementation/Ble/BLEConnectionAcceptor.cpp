//
// Created by bele on 09.08.17.
//


#include "BLEConnectionAcceptor.h"


BLEConnectionAcceptor::BLEConnectionAcceptor(BLESocket *bleSocket) : bleSocket(bleSocket), stopped(false) {}


bool BLEConnectionAcceptor::start() {
    stopped = false;
    adapterFoundFlag.reset();
    deviceStatistics.clear();
    acceptorThread = std::thread(&BLEConnectionAcceptor::loop,this);
    std::chrono::seconds sec(5);
    if(!adapterFoundFlag.wait_for(sec)){
        stop();
        return false;
    }
    return true;
}

void BLEConnectionAcceptor::stop() {
    stopped=true;
    if(acceptorThread.joinable()){
        acceptorThread.join();
    }
}

void BLEConnectionAcceptor::loop() {
    BLE ble;
    std::string adapterMac;
    while(!stopped){
        // wait for currentAdapter
        if(currentAdapter == nullptr){
            currentAdapter = waitForAdapter(ble);
            adapterMac = currentAdapter->getMac();
            currentAdapter->startScan();
            adapterFoundFlag.set();
            continue;
        }
        std::list<std::shared_ptr<BLEDevice>> bleDevices = currentAdapter->getDevices();
        for (auto &&bleDevice : bleDevices) {
            if(stopped){
                return;
            }
            std::string mac = bleDevice->getMac();
            if(bleDevice->isBroken()){
                // broken devices (RSSI and TxPower not available) are removed immediately
                currentAdapter->removeBLEDevice(bleDevice);
                continue;
            }
            if(mac.empty()){
                // empty macs are ignored
                currentAdapter->removeBLEDevice(bleDevice);
                continue;
            }
            if(deviceStatisticsContains(mac)){
                auto deviceStatistic = deviceStatistics.find(mac);
                if(deviceStatistic->second->isConnectionInProgress()){
                    // nothing to do here
                    continue;
                }
                if(deviceStatistic->second->isFaulty()){
                    // nothing to do here
                    continue;
                }
                auto connection = std::make_shared<BLEConnection>(bleSocket, currentAdapter, bleDevice, deviceStatistic->second);
                connection->connect();
            }else{
                auto deviceStatistic = std::make_shared<BLEDeviceStatistic>(mac);
                deviceStatistics.insert(std::make_pair(mac, deviceStatistic));
            }
        }
    }
}

std::shared_ptr<BLEAdapter> BLEConnectionAcceptor::waitForAdapter(BLE &ble) {
    std::shared_ptr<BLEAdapter> adapter = nullptr;
    while(!stopped){
        std::list<std::shared_ptr<BLEAdapter>> adapters = ble.getAdapters();
        if(adapters.empty()){
            continue;
        }
        adapter = adapters.front();
        return adapter;
    }
    return nullptr;
}

bool BLEConnectionAcceptor::deviceStatisticsContains(std::string mac) {
    return deviceStatistics.find(mac) != deviceStatistics.end();
}

std::string BLEConnectionAcceptor::getAdapterMac() {
    if(currentAdapter == nullptr){
        return std::string();
    }
    return currentAdapter->getMac();
}

