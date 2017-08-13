//
// Created by bele on 09.08.17.
//

#include "BLEConnection.h"
#include "BLEMessage.h"


BLEConnection::BLEConnection(BLESocket *bleSocket, const std::shared_ptr<BLEAdapter> &bleAdapter,
                             const std::shared_ptr<BLEDevice> &bleDevice,
                             const std::shared_ptr<BLEDeviceStatistic> bleDeviceStatistic) : bleSocket(bleSocket),
                                                                                             bleAdapter(bleAdapter),
                                                                                             bleDevice(bleDevice),
                                                                                             bleDeviceStatistic(
                                                                                                      bleDeviceStatistic) {}

void BLEConnection::connect() {
    bleDeviceStatistic->setConnectionInProgress();
    connectionThread = std::thread(&BLEConnection::handleConnection, shared_from_this());
}

void BLEConnection::stop() {
    signalInterruptReceived = true;
    if(connectionThread.joinable()){
        connectionThread.join();
    }
}

void BLEConnection::handleConnection() {
    if (!bleDevice->connect()) {
        bleDeviceStatistic->incrementConnectFailed();
        return;
    }
    if (!bleDevice->hasNUS()) {
        bleDeviceStatistic->incrementServiceFailed();
        return;
    }
    bleNUSConnection = bleDevice->getNUSConnection();
    if (bleNUSConnection == nullptr) {
        bleDeviceStatistic->incrementServiceFailed();
        return;
    }
    bleDeviceMac = bleDevice->getMac();
    if (bleDeviceMac.empty()) {
        // very unlikely error
        return;
    }
    bleNUSConnection->connect();
    bleDeviceStatistic->reset();

    bleSocket->addBLEConnection(shared_from_this());

    while (bleDevice->isConnected() && !signalInterruptReceived) {
        std::vector<uint8_t> msg = bleNUSConnection->getMessage();
        if (msg.empty()) {
            continue;
        }
        std::unique_ptr<BLEMessage> bleMessage(new BLEMessage(bleDeviceMac, msg));
        bleSocket->addBLEMessage(std::move(bleMessage));
    }
    bleNUSConnection->disconnect();
    bleDevice->disconnect();

    // finally
    bleSocket->removeBLEConnection(bleDeviceMac);
    bleAdapter->removeBLEDevice(bleDevice);
    bleDeviceStatistic->unsetConnectionInProgress();

    // session finished
}

void BLEConnection::send(std::vector<uint8_t> data) {
    bleNUSConnection->send(data);
}

const std::string BLEConnection::getMac() const {
    return bleDeviceMac;
}



