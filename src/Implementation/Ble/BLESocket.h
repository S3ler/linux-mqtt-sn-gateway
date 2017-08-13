//
// Created by bele on 09.08.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_BLESOCKET_H
#define LINUX_MQTT_SN_GATEWAY_BLESOCKET_H

#include "BLEMessage.h"
#include "BLEConnection.h"

#include <SocketInterface.h>
#include <memory>
#include "BLEConnectionAcceptor.h"
#include "BLEMessage.h"

class BLEConnectionAcceptor;
class BLEConnection;

class BLESocket  : public SocketInterface {
private:
    std::unique_ptr<BLEConnectionAcceptor> connectionAcceptor;
    std::mutex bleConnectionsMutex;
    std::unordered_map<std::string, std::weak_ptr<BLEConnection>> bleConnections;
    std::mutex bleMessageQueueMutex;
    std::queue<std::unique_ptr<BLEMessage>> bleMessageQueue;

    LoggerInterface *logger;
    MqttSnMessageHandler *mqttSnMessageHandler;

    device_address ownAddress;
public:

    bool begin() override;

    void setLogger(LoggerInterface *logger) override;

    void setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) override;

    device_address *getBroadcastAddress() override;

    device_address *getAddress() override;

    uint8_t getMaximumMessageLength() override;

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) override;

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) override;

    bool loop() override;

    void addBLEMessage(std::unique_ptr<BLEMessage> &&unique_ptr);

    void addBLEConnection(std::weak_ptr<BLEConnection> bleConnection);

    void removeBLEConnection(std::string bleConnectionMac);

    std::string getMacFromDeviceAddress(device_address* deviceAddress);
    device_address getDeviceAddressFromMac(std::string mac);

    ~BLESocket() override;
};


#endif //LINUX_MQTT_SN_GATEWAY_BLESOCKET_H
