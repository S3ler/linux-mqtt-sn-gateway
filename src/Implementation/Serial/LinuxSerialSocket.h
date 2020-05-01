//
// Created by bele on 03.01.18.
//

#ifndef TEST_MQTT_SN_GATEWAY_LINUXSERIALSOCKET_H
#define TEST_MQTT_SN_GATEWAY_LINUXSERIALSOCKET_H


#include <SocketInterface.h>
#include <mutex>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <climits>
#include <unistd.h>

// #define BROADCAST_ADDRESS 255
// #define OWN_ADDRESS 127
// #define MAX_MSG_LENGTH 30
enum TransmissionProtocolUartBridgeStatus {
    STARTING,
    IDLE,
    SEND,
    RECEIVE,
    CONFIGURATION,
    PARSE_FAILURE,
    ERROR
};


class LinuxSerialSocket : public SocketInterface {
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

private:
    char *portname = "/dev/ttyS0";
    int fd = -1;
    std::mutex mutex;

    LoggerInterface *logger = nullptr;
    MqttSnMessageHandler *mqttSnMessageHandler = nullptr;

    device_address ownAddress;
    device_address broadcastAddress;
    uint16_t maximumMessageLength = 0;
    uint16_t serialBufferSize = 0;


    device_address from_address;
    uint8_t data[UINT16_MAX];
    uint16_t data_length = 0;

    uint8_t status = STARTING;

    bool initSerialPort();

    int set_interface_attribs(int fd, int speed, int parity);

    int set_blocking(int fd, int should_block);

    bool resetController();

    bool updateConfiguration();

    bool readStatus();

    bool readOwnAddress();

    bool readBroadcastAddress();

    bool readMaximumMessageLength();

    bool readSerialBufferSize();

    bool waitForOKIdle();

    bool waitFor(char *toCompare);

    void waitForOkAwaitAddress();

    void sendAddress(device_address *address);

    void waitForOkAwaitData();

    void sendData(uint8_t *buf, uint8_t len);

    void waitForOkSending();

    void waitForOkSendAddress();

    void readSendAddress();

    void waitForOkSendData();

    void readData();

    bool parseLong(const char *str, long *val);
};


#endif //TEST_MQTT_SN_GATEWAY_LINUXSERIALSOCKET_H
