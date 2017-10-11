//
// Created by bele on 09.08.17.
//

#include <iomanip>
#include "BLESocket.h"

bool BLESocket::begin() {
    connectionAcceptor = std::make_unique<BLEConnectionAcceptor>(this);
    if(!connectionAcceptor->start()){
        return false;
    }
    mqttSnMessageHandler->notify_socket_connected();
    // TODO rest der initialisierung
    return true;
}

void BLESocket::setLogger(LoggerInterface *logger) {
    this->logger = logger;
}

void BLESocket::setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) {
    this->mqttSnMessageHandler = mqttSnMessageHandler;
}

device_address *BLESocket::getBroadcastAddress() {
    return nullptr;
}

device_address *BLESocket::getAddress() {
    std::string ownMac = connectionAcceptor->getAdapterMac();
    if (ownMac.empty()) {
        // this status holds when someone removes the ble adapter... or the dbus is broken
        // both cases were our application is down
        throw std::runtime_error("BLESocket::getAddress: ownMac is empty.");
    }
    ownAddress = getDeviceAddressFromMac(ownMac);
    return &ownAddress;
}

uint8_t BLESocket::getMaximumMessageLength() {
    // TODO magic number
    return 20;
}

bool BLESocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) {
    std::lock_guard<std::mutex> bleConnectionsLockGuard(bleConnectionsMutex);
    std::string destinationMac = getMacFromDeviceAddress(destination);
    auto bleConnection = bleConnections.find(destinationMac);
    if (bleConnection != bleConnections.end()) {
        if (std::shared_ptr<BLEConnection> spBLEConnection = bleConnection->second.lock()) {
            std::vector<uint8_t> vdata(bytes, bytes + bytes_len);
            spBLEConnection->send(vdata);
        } else {
            bleConnections.erase(bleConnection);
        }
    }
    return true;
}

bool BLESocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) {
    return send(destination, bytes, bytes_len);
}

bool BLESocket::loop() {
    bleMessageQueueMutex.lock();
    if (!bleMessageQueue.empty()) {
        std::unique_ptr<BLEMessage> bleMessage = std::move(bleMessageQueue.front());
        bleMessageQueue.pop();
        bleMessageQueueMutex.unlock();

        uint8_t data[255];
        memset(&data, 0, sizeof(data));
        std::vector<uint8_t> vdata = bleMessage->getData();
        if (vdata.size() > 255) {
            // too long ignore
            return true;
        }
        std::copy(vdata.begin(), vdata.end(), data);

        device_address deviceAddress = getDeviceAddressFromMac(bleMessage->getMac());

        mqttSnMessageHandler->receiveData(&deviceAddress, data);
        return true;
    }
    bleMessageQueueMutex.unlock();
    return true;
}

void BLESocket::addBLEConnection(std::shared_ptr<BLEConnection> bleConnection) {
    std::lock_guard<std::mutex> bleConnectionsLockGuard(bleConnectionsMutex);
    std::weak_ptr<BLEConnection> weakptr_bleConnection = bleConnection;
    if (auto sp = weakptr_bleConnection.lock()) {
        bleConnections.insert(std::make_pair(sp->getMac(), bleConnection));
    }
}

void BLESocket::removeBLEConnection(std::string bleConnectionMac) {
    std::lock_guard<std::mutex> bleConnectionsLockGuard(bleConnectionsMutex);
    auto bleConnection = bleConnections.find(bleConnectionMac);
    if (bleConnection != bleConnections.end()) {
        bleConnections.erase(bleConnection);
    }
}

void BLESocket::addBLEMessage(std::unique_ptr<BLEMessage> &&unique_ptr) {
    std::lock_guard<std::mutex> bleMessageQueueLockGuard(bleMessageQueueMutex);
    bleMessageQueue.push(std::move(unique_ptr));
}

std::string BLESocket::getMacFromDeviceAddress(device_address *deviceAddress) {
    // TODO check if works
    // https://stackoverflow.com/questions/7639656/getting-a-buffer-into-a-stringstream-in-hex-representation/7639754#7639754
    char macStr[17] = {0};
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::uppercase;
    for(uint8_t l = 0; l < sizeof(device_address);l++){
        ss << std::setw(2) << static_cast<unsigned>(deviceAddress->bytes[l]);
        if(l != (sizeof(device_address)-1)){
            ss << ":";
        }
    }
    return ss.str();
}

device_address BLESocket::getDeviceAddressFromMac(std::string mac) {
    if (mac.length() != 17) {
        throw std::runtime_error("BLESocket::getDeviceAddressFromMac is not 17 characters long");
    }
    // from: https://stackoverflow.com/questions/20553805/how-to-convert-a-mac-address-in-string-to-array-of-integers
    int values[6];
    device_address deviceAddress;
    char tmp_mac[17] = {0};
    strcpy(tmp_mac, mac.c_str());
    if (6 == sscanf(tmp_mac, "%x:%x:%x:%x:%x:%x%c",
                    &values[0], &values[1], &values[2],
                    &values[3], &values[4], &values[5])) {
        /* convert to uint8_t */
        for (int i = 0; i < 6; ++i)
            deviceAddress.bytes[i] = (uint8_t) values[i];
    }

    return deviceAddress;
}

BLESocket::~BLESocket() {
    connectionAcceptor->stop();

    /*{
        std::lock_guard<std::mutex> bleConnectionsLockGuard(bleConnectionsMutex);
        for (auto iterator = bleConnections.begin(); iterator != bleConnections.end(); /*++iterator*//*) {
        /*        if (auto spBLEConnection = iterator->second.lock()) {}
                else {
                    bleConnections.erase(iterator);
                }
            }
        }*/

    // TODO check if the compiler does not optimise this!
        /*while (!bleConnections.empty()) {}*/
    }


