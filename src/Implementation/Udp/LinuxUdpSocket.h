//
// Created by bele on 07.04.17.
//

#ifndef CORE_MQTT_SN_GATEWAY_SOCKETIMPLEMENTATION_H
#define CORE_MQTT_SN_GATEWAY_SOCKETIMPLEMENTATION_H


#include <SocketInterface.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <ifaddrs.h>

#define BUFLEN 255    //Max length of buffer
#define PORT 8888    //The port on which to listen for incoming data

class LinuxUdpSocket : public SocketInterface {
public:

    struct sockaddr_in udp_socket_address, si_other;

    int udp_socket, i, recv_len;
    socklen_t slen = sizeof(si_other);
    char buf[BUFLEN];

    int socket_descriptor;


    MqttSnMessageHandler *mqttsn = nullptr;
    device_address own_address;
    device_address broadcast_address;

    LoggerInterface *logger;
public:
    bool begin() override;

    bool initUdpSocket();

    bool initMulticastUdpSocket();

    void setLogger(LoggerInterface *logger) override;

    void setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) override;

    device_address *getBroadcastAddress() override;

    device_address *getAddress() override;

    uint8_t getMaximumMessageLength() override;

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) override;

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) override;

    bool loop() override;

private:
    device_address getDevice_address(sockaddr_in *addr) const;

    uint32_t getIp_address(device_address *address) const;

    uint16_t getPort(device_address *address) const;

};


#endif //CORE_MQTT_SN_GATEWAY_SOCKETIMPLEMENTATION_H
