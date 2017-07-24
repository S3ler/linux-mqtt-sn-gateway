//
// Created by bele on 22.07.17.
//

#ifndef GATTLIB_EXPERIMENTS_BLUETOOTHLOWENERGYSOCKET_H
#define GATTLIB_EXPERIMENTS_BLUETOOTHLOWENERGYSOCKET_H

#include "../Connect/Connector.h"
#include "../Connect/Connection.h"
#include <list>
#include "BluetoothLowEnergyMessage.h"
#include "Queue.h"
#include "../../../core-mqtt-sn-gateway/src/SocketInterface.h"

// make configureable
#define s_mac "00:1A:7D:DA:71:20"
#define c_mac "00:1A:7D:DA:71:21"

class BluetoothLowEnergySocket : public SocketInterface, public ReceiverInterface {
private:

    char scanner_mac[18] = {0};
    Scanner *scanner = nullptr;

    char connector_mac[18] = {0};
    Connector *connector = nullptr;

    device_address broadcast_address;
    device_address own_address;

    Queue<std::shared_ptr<BluetoothLowEnergyMessage>> receiver_queue;
    LoggerInterface *logger;
    MqttSnMessageHandler *mqttSnMessageHandler;
public:
    void setLogger(LoggerInterface *logger) override;

    void setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) override;

    BluetoothLowEnergySocket();

    BluetoothLowEnergySocket(const char *scanner_mac, const char *connector_mac);

    bool begin() override;

    device_address *getBroadcastAddress() override;

    device_address *getAddress() override;

    uint8_t getMaximumMessageLength() override;

    void onReceive(const device_address *address, const uint8_t *payload, const uint16_t payload_length) override;

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) override;

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) override;

    bool loop() override;

};


#endif //GATTLIB_EXPERIMENTS_BLUETOOTHLOWENERGYSOCKET_H
