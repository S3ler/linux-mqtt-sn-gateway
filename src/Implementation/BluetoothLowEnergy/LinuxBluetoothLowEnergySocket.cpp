//
// Created by bele on 11.06.17.
//

#include <BluetoothLowEnergy/gattlib/include/gattlib.h>
#include <iostream>
#include "LinuxBluetoothLowEnergySocket.h"

bool LinuxBluetoothLowEnergySocket::begin() {
    // TODO uncommend later
    /*
    if (logger == nullptr || mqttSnMessageHandler == nullptr) {
        return false;
    }
     */

    memset(&this->broadcast_address, 255, sizeof(device_address));
    // TODO: set own_address


    int ret;

    adapter_name = NULL;
    ret = gattlib_adapter_open(adapter_name, &adapter);
    if (ret) {
        fprintf(stderr, "unable to open ble adapter!");
        // failed
        // TODO log
        return false;
    }


    this->scanner = new Scanner();

    this->connectionCreator = new PeripherapConnectionCreator();
    this->scanner->setScannerQueue(this->connectionCreator->getQueue());
    this->connectionCreator->setBleSocket(this);

    this->connectionCreator->start_loop();
    if (!this->scanner->scan_enable(adapter)) {
        return false;
    }
    return true;
}

void LinuxBluetoothLowEnergySocket::setLogger(LoggerInterface *logger) {
    this->logger = logger;
}

void LinuxBluetoothLowEnergySocket::setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) {
    this->mqttSnMessageHandler = mqttSnMessageHandler;
}

device_address *LinuxBluetoothLowEnergySocket::getBroadcastAddress() {
    return &this->broadcast_address;
}

device_address *LinuxBluetoothLowEnergySocket::getAddress() {
    return &this->own_address;
}

uint8_t LinuxBluetoothLowEnergySocket::getMaximumMessageLength() {
    return BLUETOOTH_LE_MAX_MESSAGE_LENGTH;
}

bool LinuxBluetoothLowEnergySocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) {
    if (destination == &broadcast_address) {
        for (auto &&connection : connections) {
            connection->send(bytes, bytes_len);
        }
        return true;
    }
    std::lock_guard<std::mutex> lock_guard(connections_mutex);
    for (auto &&connection : connections) {
        if (memcmp(destination, connection->getAddress(), sizeof(device_address)) == 0) {
            return connection->send(bytes, bytes_len);
        }
    }
    return false;
}

bool LinuxBluetoothLowEnergySocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len,
                                         uint8_t signal_strength) {
    return this->send(destination, bytes, bytes_len);
}

bool LinuxBluetoothLowEnergySocket::loop() {
    if (!receiver_queue.empty()) {
        BluetoothLowEnergyMessage *msg = receiver_queue.pop();
        // FIXME
        // TODO solve:
        // /home/bele/mqttsngit/linux-mqtt-sn-gateway/src/Implementation/BluetoothLowEnergy/LinuxBluetoothLowEnergySocket.cpp:85: Nicht definierter Verweis auf `MqttSnMessageHandler::receiveData(device_address*, unsigned char*)'
        // mqttSnMessageHandler->receiveData(&msg->address, (uint8_t *) &msg->payload);
        //logger->log("received message", 0);
        char *mac = convertToMacString(&msg->address);
        std::cout << mac << " | " << msg->payload << std::endl;
        this->send(&msg->address, msg->payload, msg->payload_length);
        delete (msg);
    }
    return true;
}


char *LinuxBluetoothLowEnergySocket::convertToMacString(device_address *address) {
    memset(&macStr, 0, sizeof(macStr));
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             address->bytes[0], address->bytes[1], address->bytes[2], address->bytes[3], address->bytes[4],
             address->bytes[5]);
    return macStr;
}


bool LinuxBluetoothLowEnergySocket::isConnected(char *mac) {
    std::lock_guard<std::mutex> lock_guard(connections_mutex);
    for (auto &&connection : connections) {
        if (strcmp(connection->getMAC(), mac) == 0) {
            return true;
        }
    }
    return false;
}

void LinuxBluetoothLowEnergySocket::addPeripheralConnection(PerpheralConnection *connection) {
    std::lock_guard<std::mutex> lock_guard(connections_mutex);
    connections.push_back(connection);
}

void LinuxBluetoothLowEnergySocket::receive(const char *mac, const uint8_t *data, size_t data_length) {
    BluetoothLowEnergyMessage *msg = new BluetoothLowEnergyMessage();
    device_address address = convertToDeviceAddress(mac);
    msg->setDeviceAddress(&address);
    if (data_length > UINT16_MAX) {
        throw std::runtime_error(
                std::string("data_length from ") + std::string(mac) + std::string(" exceeds maximum of UINT16_MAX."));
    }
    msg->setPayload(data, (const uint16_t) data_length);
    receiver_queue.push(msg);
}

device_address LinuxBluetoothLowEnergySocket::convertToDeviceAddress(const char *mac) {
    unsigned int bytes[6];
    if (std::sscanf(mac,
                    "%02x:%02x:%02x:%02x:%02x:%02x",
                    &bytes[0], &bytes[1], &bytes[2],
                    &bytes[3], &bytes[4], &bytes[5]) != 6) {
        throw std::runtime_error(std::string(mac) + std::string(" is an invalid MAC address"));
    }
    device_address result = device_address();
    for (uint8_t i = 0; i < 6; i++) {
        if (bytes[i] > UINT8_MAX) {
            throw std::runtime_error(std::string(mac) + std::string(" is an invalid MAC address"));
        }
        result.bytes[i] = (uint8_t) bytes[i];
    }
    return result;
}

LinuxBluetoothLowEnergySocket::~LinuxBluetoothLowEnergySocket() {
    // TODO muss der scanner überhaupt deleted werden?
    delete scanner;

    this->connectionCreator->stop_loop();

    std::lock_guard<std::mutex> lock_guard(connections_mutex);
    for (auto &&connection : connections) {
        delete connection;
    }

    /*
     * ein problem für folgenden fall bleibt weiterhin bestehen:
     * alles wird heruntergefahren:
     * con-creater hat noch eine connection erstellt - diese braucht sehr laner
     * => weder in connections list noch irgendwo anders
     * => wird nie heruntergefahren
     */

    gattlib_adapter_close(adapter);
}
