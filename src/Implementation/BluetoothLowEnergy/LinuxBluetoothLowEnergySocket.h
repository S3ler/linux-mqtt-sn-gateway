//
// Created by bele on 11.06.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_LINUXBLUETOOTHLESOCKET_H
#define LINUX_MQTT_SN_GATEWAY_LINUXBLUETOOTHLESOCKET_H


#include <LoggerInterface.h>
#include <MqttSnMessageHandler.h>
#include "Queue.h"
#include "Scanner.h"
#include "PeripherapConnectionCreator.h"
#include "PerpheralConnection.h"
#include "BluetoothLowEnergyMessage.h"
#include "BluetoothLowEnergyAdvertise.h"
#include <list>

#define BLUETOOTH_LE_MAX_MESSAGE_LENGTH 20

class PerpheralConnection;
class PeripherapConnectionCreator;

class LinuxBluetoothLowEnergySocket : public SocketInterface {
private:
    LoggerInterface *logger = nullptr;
    MqttSnMessageHandler *mqttSnMessageHandler = nullptr;
    device_address broadcast_address;
    device_address own_address;
    Queue<BluetoothLowEnergyMessage *> receiver_queue;

    std::list<PerpheralConnection *> connections;
    std::mutex connections_mutex;

    const char* adapter_name = NULL;
    void* adapter;

    char macStr[19];

    Scanner *scanner;
    PeripherapConnectionCreator *connectionCreator;

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

    bool isConnected(char *mac);

    void addPeripheralConnection(PerpheralConnection *connection);

    void receive(const char* mac,const uint8_t *data, size_t data_length);

    device_address convertToDeviceAddress(const char *mac);

    char* convertToMacString(device_address* address);

        ~LinuxBluetoothLowEnergySocket();
};





#endif //LINUX_MQTT_SN_GATEWAY_LINUXBLUETOOTHLESOCKET_H
