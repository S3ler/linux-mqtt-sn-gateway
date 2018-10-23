//
// Created by bele on 13.06.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_TCPSOCKET_H
#define LINUX_MQTT_SN_GATEWAY_TCPSOCKET_H


#include <SocketInterface.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <ifaddrs.h>


#define BUFLEN 255    //Max length of buffer
#define PORT 8888     //The port on which to listen for incoming data

class SingleTcpSocket : public SocketInterface {
private:
    struct sockaddr_in tcp_socket_address,tcp_socket_address_len, si_other;
    int tcp_socket, i, recv_len;
    const char *network_interface = "eth0";

    long save_fd;
    struct sockaddr_in connection_socket_address,  connection_si_other;
    int connection_socket, connection_i, connection_recv_len;
     socklen_t connection_socket_address_len;

    device_address connection_address;
    device_address broadcast_address;
    device_address own_address;

    MqttSnMessageHandler *mqttSnMessageHandler;
    LoggerInterface *logger;

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

    bool initTcpSocket();

private:
    device_address getDevice_address(sockaddr_in *addr) const;

    uint32_t getIp_address(device_address *address) const;

    uint16_t getPort(device_address *address) const;
};


#endif //LINUX_MQTT_SN_GATEWAY_TCPSOCKET_H
