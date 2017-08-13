//
// Created by bele on 22.07.17.
//

#include <LoggerInterface.h>
#include <MqttSnMessageHandler.h>
#include "BluetoothLowEnergySocket.h"

bool BluetoothLowEnergySocket::begin() {
    // TODO set own_address as connector_mac;

    this->scanner = new Scanner(scanner_mac);
    this->connector = new Connector(connector_mac, scanner);
    this->connector->setReceiverInterface(this);
    this->scanner->scan(this->connector);

    while (!this->scanner->isRunning()) { /* wait until scanner is running */ }
    this->connector->start();
    // now everything for bluetooth runs and is fine
    // TODO wie stelle ich fest ob auch die macs passen?!
    mqttSnMessageHandler->notify_socket_connected();
    return true;
}

device_address *BluetoothLowEnergySocket::getBroadcastAddress() {
    return &broadcast_address;
}

device_address *BluetoothLowEnergySocket::getAddress() {
    return &own_address;
}

uint8_t BluetoothLowEnergySocket::getMaximumMessageLength() {
    return 20;
}

bool BluetoothLowEnergySocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) {
    if (destination == &broadcast_address) {
        // send to all!
    }
    if (!connector->isConnected(destination)) {
        // TODO disconnect device?
        return true;
    }
    if (!connector->send(destination, bytes, bytes_len)) {
        // TODO disconnect device?
        return true;
    }
    return true;
}

bool BluetoothLowEnergySocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len,
                                    uint8_t signal_strength) {
    return this->send(destination, bytes, bytes_len);
}

bool BluetoothLowEnergySocket::loop() {
    if (!receiver_queue.empty()) {

        std::shared_ptr<BluetoothLowEnergyMessage> msg = receiver_queue.pop();
        device_address *address = (device_address *) &msg->getAddress();
        uint8_t data[255] = {0};
        memcpy(&data, msg->getPayload(), 255);
        mqttSnMessageHandler->receiveData((device_address *) &msg->getAddress(), (uint8_t *) msg->getPayload());
        /*
        const device_address address = msg.get()->getAddress();
        const char *data = (const char *) msg.get()->getPayload();
        const uint16_t length = msg.get()->getPayload_length();
        for (int i = 0; i < sizeof(device_address); i++) {
            std::cout << std::hex << std::uppercase << (int) address.bytes[i]
                      << std::nouppercase << std::dec
                      << std::flush;
            if (i != sizeof(device_address) - 1) {
                std::cout << ":";
            }
        }
        std::cout << " - ";
        for (int i = 0; i < length; i++) {
            std::cout << (char) data[i] << std::flush;
        }
        std::cout << std::endl;
        */
    }
    return true;
}

void BluetoothLowEnergySocket::onReceive(const device_address *address, const uint8_t *payload,
                                         const uint16_t payload_length) {
    if (payload_length > getMaximumMessageLength()) {
        // TODO log
        // ignore
    }
    std::shared_ptr<BluetoothLowEnergyMessage> msg(new BluetoothLowEnergyMessage(address, payload, payload_length));
    receiver_queue.push(msg);
}


BluetoothLowEnergySocket::BluetoothLowEnergySocket() : BluetoothLowEnergySocket(s_mac, c_mac) {}

BluetoothLowEnergySocket::BluetoothLowEnergySocket(const char *scanner_mac, const char *connector_mac) {
    if (scanner_mac == nullptr) {
        // TODO error
    }
    if (connector_mac == nullptr) {
        // TODO error
    }
    if (strlen(scanner_mac) != 17) {
        // TODO error
    }
    if (strlen(connector_mac) != 17) {
        // TODO error
    }
    strcpy(this->scanner_mac, scanner_mac);
    strcpy(this->connector_mac, connector_mac);
    memset(&broadcast_address, 0xFF, sizeof(device_address));
}

void BluetoothLowEnergySocket::setLogger(LoggerInterface *logger) {
    if (logger == nullptr) {
        //TODO error
    }
    this->logger = logger;
}

void BluetoothLowEnergySocket::setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) {
    if (mqttSnMessageHandler == nullptr) {
        //TODO error
    }
    this->mqttSnMessageHandler = mqttSnMessageHandler;
}


