//
// Created by bele on 09.08.17.
//

#include "BLEConnection.h"
#include "BLEMessage.h"


BLEConnection::BLEConnection(BLESocket *bleSocket, const std::shared_ptr<BLEAdapter> &bleAdapter,
                             const std::shared_ptr<BLEDevice> &bleDevice,
                             const std::shared_ptr<BLEDeviceStatistic> bleDeviceStatistic)
        : bleSocket(bleSocket),
          bleAdapter(bleAdapter),
          bleDevice(bleDevice),
          bleDeviceStatistic(bleDeviceStatistic),
          signalInterruptReceived(false)
{}

void BLEConnection::connect() {
    bleDeviceStatistic->setConnectionInProgress();
    //connectionThread = std::make_shared<std::thread>(&BLEConnection::handleConnection, shared_from_this());
    connectionThread = new std::thread(&BLEConnection::handleConnection, shared_from_this(),shared_from_this());
}

void BLEConnection::stop() {
    signalInterruptReceived = true;
    if(connectionThread->joinable()){
        connectionThread->join();
    }
}

void BLEConnection::handleConnection(std::shared_ptr<BLEConnection> shared_this) {
    if (!bleDevice->connect()) {
        bleDeviceStatistic->incrementConnectFailed();
        bleAdapter->removeBLEDevice(bleDevice);
        bleDeviceStatistic->unsetConnectionInProgress();
        return;
    }
    if (!bleDevice->hasNUS()) {
        bleDeviceStatistic->incrementServiceFailed();
        bleAdapter->removeBLEDevice(bleDevice);
        bleDeviceStatistic->unsetConnectionInProgress();
        return;
    }
    if(bleDevice->hasMultipleNUS()){
        std::list<std::shared_ptr<BLENUSConnection>> bleNUSConnections = bleDevice->getMultipleNUSConnections();
        if(bleNUSConnections.empty()){
            bleNUSConnection = nullptr;
        }
        // now read all
        for (auto &&connection : bleNUSConnections) {
            auto lastMsg = connection->getLastMessage();
            if(!lastMsg.empty()){
                bleNUSConnection = connection;
                connection->getMessage();
                break;
            }
            // deque the first message
        }
    }else{
        bleNUSConnection = bleDevice->getNUSConnection();
        auto lastMsg = bleNUSConnection->getLastMessage();
        if(!lastMsg.empty()){
            bleNUSConnection->getMessage();
        }
        bleNUSConnection = nullptr;
    }
    if (bleNUSConnection == nullptr) {
        bleDeviceStatistic->incrementServiceFailed();
        bleAdapter->removeBLEDevice(bleDevice);
        bleDeviceStatistic->unsetConnectionInProgress();
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
    try{
        bleNUSConnection->disconnect();
        bleDevice->disconnect();

        // finally
        bleSocket->removeBLEConnection(bleDeviceMac);
        bleAdapter->removeBLEDevice(bleDevice);
    }catch(std::runtime_error e){

    }
    bleDeviceStatistic->unsetConnectionInProgress();
    // session finished
}

void BLEConnection::send(std::vector<uint8_t> data) {
    bleNUSConnection->send(data);
}

const std::string BLEConnection::getMac() const {
    return bleDeviceMac;
}

BLEConnection::~BLEConnection() {

}



